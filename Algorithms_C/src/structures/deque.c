#include "algorithms_c/structures/deque.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * The deque implementation mirrors the semantics of the Python ``Deque``
 * class but embraces a ring-buffer storage layout so that push and pop
 * operations stay amortised ``O(1)``.  The implementation code purposefully
 * documents every decision point; the comments are intentionally verbose so
 * that readers can reason about the invariants without needing to
 * cross-reference the header file.
 */

/** Allocate a raw buffer while guarding against overflow. */
static void *allocate_buffer(size_t count, size_t element_size) {
    if (count == 0 || element_size == 0) {
        return NULL;
    }
    if (count > SIZE_MAX / element_size) {
        errno = ENOMEM;
        return NULL;
    }
    return malloc(count * element_size);
}

/**
 * Resize helper that ensures ``deque`` has enough capacity for
 * ``min_capacity`` elements.  The growth policy doubles the allocation which
 * keeps amortised operations constant.
 */
static int ensure_capacity(ac_deque *deque, size_t min_capacity) {
    size_t new_capacity;

    if (deque->capacity >= min_capacity) {
        return 0;
    }

    new_capacity = deque->capacity == 0 ? 1U : deque->capacity;
    while (new_capacity < min_capacity) {
        if (new_capacity > SIZE_MAX / 2U) {
            return -ENOMEM;
        }
        new_capacity *= 2U;
    }

    return ac_deque_reserve(deque, new_capacity);
}

/*
 * Short description: initialize deque storage and ring indices.
 * Long description: initialization allocates optional initial capacity and
 * sets a canonical empty state (size=0, head=tail=0), mirroring constructor
 * semantics of the Python Deque structure while making ownership explicit.
 * Signature: int ac_deque_init(ac_deque *deque, size_t element_size,
 *                              size_t initial_capacity)
 */
int ac_deque_init(
    ac_deque *deque,
    size_t element_size,
    size_t initial_capacity
) {
    if (deque == NULL || element_size == 0) {
        return -EINVAL;
    }

    deque->data = allocate_buffer(initial_capacity, element_size);
    if (initial_capacity != 0 && deque->data == NULL) {
        return -ENOMEM;
    }

    deque->element_size = element_size;
    deque->capacity = initial_capacity;
    deque->size = 0;
    deque->head = 0;
    deque->tail = 0;
    return 0;
}

/*
 * Short description: free deque resources and reset metadata.
 * Long description: destruction releases the backing buffer and clears fields
 * so repeated init/destroy cycles in tests remain deterministic. NULL is
 * accepted as a defensive no-op to simplify teardown paths. Signature: void
 * ac_deque_destroy(ac_deque *deque)
 */
void ac_deque_destroy(ac_deque *deque) {
    if (deque == NULL) {
        return;
    }

    free(deque->data);
    deque->data = NULL;
    deque->element_size = 0;
    deque->capacity = 0;
    deque->size = 0;
    deque->head = 0;
    deque->tail = 0;
}

/*
 * Short description: report whether deque holds zero elements.
 * Long description: emptiness is determined from explicit size tracking rather
 * than head/tail comparison, avoiding ambiguous states in ring-buffer layouts.
 * NULL is treated as empty for defensive callers.
 * Signature: bool ac_deque_empty(const ac_deque *deque)
 */
bool ac_deque_empty(const ac_deque *deque) {
    return deque == NULL || deque->size == 0;
}

/*
 * Short description: return logical element count.
 * Long description: size is maintained eagerly on push/pop, so this accessor is
 * O(1) and useful for tests asserting invariants after random operations.
 * Signature: size_t ac_deque_size(const ac_deque *deque)
 */
size_t ac_deque_size(const ac_deque *deque) {
    return deque == NULL ? 0U : deque->size;
}

/*
 * Short description: return allocated slot capacity.
 * Long description: capacity reflects reserved storage, not current size. This
 * is primarily surfaced for diagnostics and growth-behavior stress checks.
 * Signature: size_t ac_deque_capacity(const ac_deque *deque)
 */
size_t ac_deque_capacity(const ac_deque *deque) {
    return deque == NULL ? 0U : deque->capacity;
}

/*
 * Short description: grow underlying storage to at least `new_capacity`.
 * Long description: reserve linearizes logical order into a fresh buffer, then
 * re-bases indices (head=0, tail=size). This makes subsequent wrap-around math
 * predictable and keeps semantics identical to Python's abstract sequence view.
 * Signature: int ac_deque_reserve(ac_deque *deque, size_t new_capacity)
 */
int ac_deque_reserve(ac_deque *deque, size_t new_capacity) {
    void *new_data;
    unsigned char *dst;
    unsigned char *src;
    size_t old_capacity;
    size_t i;
    size_t index;

    if (deque == NULL) {
        return -EINVAL;
    }

    if (new_capacity <= deque->capacity) {
        return 0;
    }

    new_data = allocate_buffer(new_capacity, deque->element_size);
    if (new_data == NULL) {
        return -ENOMEM;
    }

    if (deque->size > 0 && deque->data != NULL) {
        dst = (unsigned char *)new_data;
        src = (unsigned char *)deque->data;
        old_capacity = deque->capacity == 0 ? 1U : deque->capacity;
        for (i = 0; i < deque->size; ++i) {
            index = (deque->head + i) % old_capacity;
            memcpy(
                dst + (i * deque->element_size),
                src + (index * deque->element_size), deque->element_size
            );
        }
    }

    free(deque->data);
    deque->data = new_data;
    deque->capacity = new_capacity;
    deque->head = 0;
    deque->tail = deque->size;
    return 0;
}

/*
 * Short description: append one element at logical back.
 * Long description: push_back ensures capacity, writes at tail, and advances
 * tail modulo capacity, preserving amortized O(1) insertion in ring layout.
 * Signature: int ac_deque_push_back(ac_deque *deque, const void *value)
 */
int ac_deque_push_back(ac_deque *deque, const void *value) {
    int status;
    unsigned char *base;

    if (deque == NULL || value == NULL) {
        return -EINVAL;
    }

    status = ensure_capacity(deque, deque->size + 1U);
    if (status != 0) {
        return status;
    }

    if (deque->capacity == 0) {
        return -ENOMEM;
    }

    base = (unsigned char *)deque->data;
    memcpy(
        base + (deque->tail * deque->element_size), value, deque->element_size
    );
    deque->tail = (deque->tail + 1U) % deque->capacity;
    deque->size++;
    return 0;
}

/*
 * Short description: prepend one element at logical front.
 * Long description: push_front ensures capacity, rewinds head with wrap-around,
 * writes value into the new head slot, and increments size. This is the dual of
 * push_back and keeps deque-end operations constant-time amortized.
 * Signature: int ac_deque_push_front(ac_deque *deque, const void *value)
 */
int ac_deque_push_front(ac_deque *deque, const void *value) {
    int status;
    unsigned char *base;

    if (deque == NULL || value == NULL) {
        return -EINVAL;
    }

    status = ensure_capacity(deque, deque->size + 1U);
    if (status != 0) {
        return status;
    }

    if (deque->capacity == 0) {
        return -ENOMEM;
    }

    deque->head = (deque->head == 0U) ? deque->capacity - 1U : deque->head - 1U;
    base = (unsigned char *)deque->data;
    memcpy(
        base + (deque->head * deque->element_size), value, deque->element_size
    );
    deque->size++;
    return 0;
}

/*
 * Short description: remove front element and optionally copy it out.
 * Long description: pop_front reads current head (if requested), advances head
 * modulo capacity, decrements size, and normalizes indices to zero when empty
 * to keep post-condition state deterministic for debugging and tests.
 * Signature: int ac_deque_pop_front(ac_deque *deque, void *out_value)
 */
int ac_deque_pop_front(ac_deque *deque, void *out_value) {
    unsigned char *base;

    if (deque == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0U) {
        return -ENOENT;
    }

    if (out_value != NULL) {
        base = (unsigned char *)deque->data;
        memcpy(
            out_value, base + (deque->head * deque->element_size),
            deque->element_size
        );
    }

    deque->head = (deque->head + 1U) % deque->capacity;
    deque->size--;
    if (deque->size == 0U) {
        deque->head = 0U;
        deque->tail = 0U;
    }
    return 0;
}

/*
 * Short description: remove back element and optionally copy it out.
 * Long description: pop_back rewinds tail with wrap-around, then reads that
 * slot as the removed element. Like pop_front, it resets indices when the deque
 * becomes empty for deterministic empty-state invariants.
 * Signature: int ac_deque_pop_back(ac_deque *deque, void *out_value)
 */
int ac_deque_pop_back(ac_deque *deque, void *out_value) {
    unsigned char *base;

    if (deque == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0U) {
        return -ENOENT;
    }

    deque->tail = (deque->tail == 0U) ? deque->capacity - 1U : deque->tail - 1U;
    if (out_value != NULL) {
        base = (unsigned char *)deque->data;
        memcpy(
            out_value, base + (deque->tail * deque->element_size),
            deque->element_size
        );
    }

    deque->size--;
    if (deque->size == 0U) {
        deque->head = 0U;
        deque->tail = 0U;
    }
    return 0;
}

/*
 * Short description: inspect front element without mutation.
 * Long description: front copies from head slot and leaves indices unchanged,
 * enabling read-only observation of deque state in algorithm traces and tests.
 * Signature: int ac_deque_front(const ac_deque *deque, void *out_value)
 */
int ac_deque_front(const ac_deque *deque, void *out_value) {
    const unsigned char *base;

    if (deque == NULL || out_value == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0U) {
        return -ENOENT;
    }

    base = (const unsigned char *)deque->data;
    memcpy(
        out_value, base + (deque->head * deque->element_size),
        deque->element_size
    );
    return 0;
}

/*
 * Short description: inspect back element without mutation.
 * Long description: back computes the tail-predecessor index with wrap-around
 * and copies that element, preserving deque state exactly.
 * Signature: int ac_deque_back(const ac_deque *deque, void *out_value)
 */
int ac_deque_back(const ac_deque *deque, void *out_value) {
    size_t index;
    const unsigned char *base;

    if (deque == NULL || out_value == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0U) {
        return -ENOENT;
    }

    index = (deque->tail == 0U) ? deque->capacity - 1U : deque->tail - 1U;
    base = (const unsigned char *)deque->data;
    memcpy(
        out_value, base + (index * deque->element_size), deque->element_size
    );
    return 0;
}

/*
 * Short description: clear contents while retaining allocated capacity.
 * Long description: clear resets logical size and both indices to zero but
 * keeps the backing buffer for future pushes, mirroring common deque clear
 * semantics. Signature: void ac_deque_clear(ac_deque *deque)
 */
void ac_deque_clear(ac_deque *deque) {
    if (deque == NULL) {
        return;
    }

    deque->size = 0;
    deque->head = 0;
    deque->tail = 0;
}
