#include "algorithms_c/structures/heap.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * The Python ``Heap`` implementation leans on explanatory docstrings to outline
 * the sift-up and sift-down mechanics that preserve the min-heap property.  The
 * C translation keeps the same verbose tone with inline commentary that mirrors
 * those explanations step by step so readers can follow the control flow and
 * invariants even without consulting the Python source.
 */

/** Simple helper to access the raw byte pointer for an index. */
static unsigned char *index_ptr(ac_heap *heap, size_t index) {
    return (unsigned char *)ac_vector_data(&heap->storage) +
           (index * heap->storage.element_size);
}

/** Swap two elements in-place while defending against large element sizes. */
static int swap_elements(ac_heap *heap, size_t lhs, size_t rhs) {
    size_t element_size = heap->storage.element_size;
    unsigned char stack_buffer[256];
    unsigned char *tmp = stack_buffer;

    if (lhs == rhs) {
        return 0;
    }

    if (element_size > sizeof(stack_buffer)) {
        tmp = (unsigned char *)malloc(element_size);
        if (tmp == NULL) {
            return -ENOMEM;
        }
    }

    unsigned char *a = index_ptr(heap, lhs);
    unsigned char *b = index_ptr(heap, rhs);

    memcpy(tmp, a, element_size);
    memcpy(a, b, element_size);
    memcpy(b, tmp, element_size);

    if (tmp != stack_buffer) {
        free(tmp);
    }

    return 0;
}

/** Restore the heap property by sifting a node towards the root. */
static int sift_up(ac_heap *heap, size_t index) {
    while (index > 0) {
        size_t parent = (index - 1U) / 2U;
        void *child_ptr = index_ptr(heap, index);
        void *parent_ptr = index_ptr(heap, parent);
        if (heap->compare(child_ptr, parent_ptr) < 0) {
            int status = swap_elements(heap, index, parent);
            if (status != 0) {
                return status;
            }
            index = parent;
        } else {
            break;
        }
    }

    return 0;
}

/** Restore the heap property by sifting a node down towards the leaves. */
static int sift_down(ac_heap *heap, size_t index) {
    size_t size = ac_heap_size(heap);

    while (1) {
        size_t left = (2U * index) + 1U;
        size_t right = left + 1U;
        size_t smallest = index;

        if (left < size &&
            heap->compare(index_ptr(heap, left), index_ptr(heap, smallest)) <
                0) {
            smallest = left;
        }

        if (right < size &&
            heap->compare(index_ptr(heap, right), index_ptr(heap, smallest)) <
                0) {
            smallest = right;
        }

        if (smallest == index) {
            break;
        }

        int status = swap_elements(heap, index, smallest);
        if (status != 0) {
            return status;
        }
        index = smallest;
    }

    return 0;
}

/*
 * Short description: initialize an empty min-heap with comparator.
 * Long description: the heap delegates storage allocation to the vector backend
 * and records the comparator used to enforce ordering. Valid element size and
 * comparator are required so subsequent sift operations are well-defined.
 * Signature: int ac_heap_init(ac_heap *heap, size_t element_size,
 *                             ac_compare_fn compare)
 */
int ac_heap_init(ac_heap *heap, size_t element_size, ac_compare_fn compare) {
    if (heap == NULL || element_size == 0 || compare == NULL) {
        return -EINVAL;
    }

    heap->compare = compare;
    return ac_vector_init(&heap->storage, element_size) == AC_VECTOR_OK
               ? 0
               : -ENOMEM;
}

/*
 * Short description: initialize heap with pre-reserved capacity.
 * Long description: this constructor mirrors Python-style preallocation paths
 * for performance-sensitive code by reserving storage up-front while preserving
 * the same comparator-driven min-heap semantics.
 * Signature: int ac_heap_with_capacity(ac_heap *heap, size_t element_size,
 *                                      size_t capacity,
 *                                      ac_compare_fn compare)
 */
int ac_heap_with_capacity(
    ac_heap *heap,
    size_t element_size,
    size_t capacity,
    ac_compare_fn compare
) {
    if (heap == NULL || element_size == 0 || compare == NULL) {
        return -EINVAL;
    }

    heap->compare = compare;
    return ac_vector_with_capacity(&heap->storage, element_size, capacity) ==
                   AC_VECTOR_OK
               ? 0
               : -ENOMEM;
}

static int heapify(ac_heap *heap) {
    /* Bottom-up heapify mirrors the Python constructor that ingests a list. */
    size_t size = ac_heap_size(heap);
    if (size == 0) {
        return 0;
    }

    size_t index = (size - 1U) / 2U + 1U;
    while (index > 0) {
        index--;
        int status = sift_down(heap, index);
        if (status != 0) {
            return status;
        }
    }

    return 0;
}

/*
 * Short description: build heap from an existing array copy.
 * Long description: values are copied into vector storage and then heapified
 * bottom-up. This keeps build complexity O(n) and matches educational
 * constructor variants shown in Python references.
 * Signature: int ac_heap_from_array(ac_heap *heap, size_t element_size,
 *                                   const void *source, size_t count,
 *                                   ac_compare_fn compare)
 */
int ac_heap_from_array(
    ac_heap *heap,
    size_t element_size,
    const void *source,
    size_t count,
    ac_compare_fn compare
) {
    if (heap == NULL || element_size == 0 || compare == NULL) {
        return -EINVAL;
    }

    heap->compare = compare;
    int status =
        ac_vector_from_array(&heap->storage, element_size, source, count);
    if (status != AC_VECTOR_OK) {
        return -ENOMEM;
    }

    return heapify(heap);
}

/*
 * Short description: destroy heap storage and reset comparator pointer.
 * Long description: destruction delegates memory release to vector destroy and
 * nulls comparator to make post-destroy misuse easier to diagnose in tests.
 * NULL heaps are accepted as a defensive no-op.
 * Signature: void ac_heap_destroy(ac_heap *heap)
 */
void ac_heap_destroy(ac_heap *heap) {
    if (heap == NULL) {
        return;
    }

    ac_vector_destroy(&heap->storage);
    heap->compare = NULL;
}

/*
 * Short description: return current number of elements.
 * Long description: size is tracked by vector backend and exposed in O(1), with
 * NULL treated as zero to simplify diagnostic call sites.
 * Signature: size_t ac_heap_size(const ac_heap *heap)
 */
size_t ac_heap_size(const ac_heap *heap) {
    return heap != NULL ? ac_vector_size(&heap->storage) : 0U;
}

/*
 * Short description: return allocated element capacity.
 * Long description: capacity reports reserved slots in underlying storage and
 * is useful in growth-behavior tests and stress instrumentation.
 * Signature: size_t ac_heap_capacity(const ac_heap *heap)
 */
size_t ac_heap_capacity(const ac_heap *heap) {
    return heap != NULL ? ac_vector_capacity(&heap->storage) : 0U;
}

/*
 * Short description: check whether heap contains no elements.
 * Long description: emptiness derives from explicit size, avoiding additional
 * state flags and keeping checks constant time.
 * Signature: bool ac_heap_empty(const ac_heap *heap)
 */
bool ac_heap_empty(const ac_heap *heap) {
    return ac_heap_size(heap) == 0U;
}

/*
 * Short description: ensure storage can hold at least `new_capacity` elements.
 * Long description: reserve forwards to vector reserve and keeps existing heap
 * layout intact; only storage capacity changes, not logical ordering.
 * Signature: int ac_heap_reserve(ac_heap *heap, size_t new_capacity)
 */
int ac_heap_reserve(ac_heap *heap, size_t new_capacity) {
    if (heap == NULL) {
        return -EINVAL;
    }

    return ac_vector_reserve(&heap->storage, new_capacity) == AC_VECTOR_OK
               ? 0
               : -ENOMEM;
}

/*
 * Short description: insert one element while preserving min-heap property.
 * Long description: insertion appends at tail then performs sift-up until
 * parent ordering is restored, giving O(log n) worst-case and amortized O(1)
 * growth. Signature: int ac_heap_push(ac_heap *heap, const void *element)
 */
int ac_heap_push(ac_heap *heap, const void *element) {
    if (heap == NULL || element == NULL) {
        return -EINVAL;
    }

    int status = ac_vector_push_back(&heap->storage, element);
    if (status != AC_VECTOR_OK) {
        return -ENOMEM;
    }

    status = sift_up(heap, ac_heap_size(heap) - 1U);
    if (status != 0) {
        return status;
    }

    return 0;
}

/*
 * Short description: copy current minimum element without removal.
 * Long description: peek reads root element at index 0 and leaves structure
 * unchanged, enabling non-destructive observation in algorithms and tests.
 * Signature: int ac_heap_peek(const ac_heap *heap, void *out_element)
 */
int ac_heap_peek(const ac_heap *heap, void *out_element) {
    if (heap == NULL || out_element == NULL || ac_heap_empty(heap)) {
        return -ENOENT;
    }

    memcpy(
        out_element, ac_vector_const_data(&heap->storage),
        heap->storage.element_size
    );
    return 0;
}

/*
 * Short description: remove minimum element and optionally copy it out.
 * Long description: pop swaps root with tail, removes tail in O(1), then
 * restores ordering via sift-down from root. This is canonical array-heap
 * deletion and keeps complexity O(log n).
 * Signature: int ac_heap_pop(ac_heap *heap, void *out_element)
 */
int ac_heap_pop(ac_heap *heap, void *out_element) {
    if (heap == NULL || ac_heap_empty(heap)) {
        return -ENOENT;
    }

    /* Swap root with the last element so we can pop_back in O(1). */
    size_t last_index = ac_heap_size(heap) - 1U;
    if (swap_elements(heap, 0U, last_index) != 0) {
        return -ENOMEM;
    }

    int status = ac_vector_pop_back(&heap->storage, out_element);
    if (status != AC_VECTOR_OK) {
        return -ENOENT;
    }

    /* Restore ordering if elements remain. */
    if (!ac_heap_empty(heap)) {
        int sift_status = sift_down(heap, 0U);
        if (sift_status != 0) {
            return sift_status;
        }
    }

    return 0;
}

/*
 * Short description: clear all elements while retaining allocation.
 * Long description: clear resets logical size through vector backend so future
 * pushes reuse allocated memory and avoid immediate reallocations.
 * Signature: void ac_heap_clear(ac_heap *heap)
 */
void ac_heap_clear(ac_heap *heap) {
    if (heap == NULL) {
        return;
    }

    ac_vector_clear(&heap->storage);
}

/*
 * Short description: compute number of levels in implicit heap tree.
 * Long description: height is derived from element count by repeated halving,
 * matching the conceptual binary-tree representation used in explanations.
 * Signature: size_t ac_heap_height(const ac_heap *heap)
 */
size_t ac_heap_height(const ac_heap *heap) {
    size_t size = ac_heap_size(heap);
    size_t height = 0U;

    while (size != 0U) {
        size /= 2U;
        height++;
    }

    return height;
}
