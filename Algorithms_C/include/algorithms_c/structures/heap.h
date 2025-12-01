#ifndef ALGORITHMS_C_STRUCTURES_HEAP_H
#define ALGORITHMS_C_STRUCTURES_HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include "algorithms_c/algorithms/binary_search.h"
#include "algorithms_c/algorithms/compare.h"
#include "algorithms_c/structures/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file heap.h
 * @brief Binary min-heap translated from the Python reference implementation.
 *
 * The original repository documents its ``Heap`` class and supporting sift
 * routines with lengthy docstrings that explain each design decision.  This
 * header mirrors that philosophy for the C translation: every API surface is
 * accompanied by both a concise summary and an extended description of the
 * underlying algorithm, invariants, and expected failure modes.  Each function
 * repeats its signature explicitly so that readers skimming the header have the
 * same contextual breadcrumbs as they would in the Python source.  The goal is
 * an educational, verbose reference whose prose outweighs the code itself.
 */

/**
 * @struct ac_heap
 * @brief Generic binary min-heap storing elements in a contiguous array.
 *
 * ``storage`` owns the backing buffer, while ``compare`` defines the ordering
 * (a negative result means the first argument is smaller).  The heap property
 * requires ``compare(child, parent) >= 0`` for every parent/child relationship,
 * ensuring that the minimum element is always at index ``0``.
 */
typedef struct {
    /** Contiguous dynamic storage reused from the generic vector. */
    ac_vector storage;
    /** Comparison callback used to maintain the heap order. */
    ac_compare_fn compare;
} ac_heap;

/**
 * @brief Initialise an empty heap with lazy allocation.
 *
 * The function mirrors ``Heap.__init__`` from the Python code: the heap starts
 * empty, records the size of each element, and defers allocation until the
 * first insertion.  ``compare`` must return a negative value when the first
 * operand is "less than" the second, establishing a min-heap ordering.
 *
 * @param heap Destination heap to initialise (must not be ``NULL``).
 * @param element_size Size in bytes of each element; must be non-zero.
 * @param compare Comparator describing the ordering; must not be ``NULL``.
 * @return ``0`` on success or ``-EINVAL`` when arguments are invalid.
 * @signature int ac_heap_init(ac_heap *heap, size_t element_size,
 *                             ac_compare_fn compare)
 */
int ac_heap_init(ac_heap *heap, size_t element_size, ac_compare_fn compare);

/**
 * @brief Initialise a heap and reserve ``capacity`` slots immediately.
 *
 * Useful when the caller knows the workload in advance.  Allocation failures
 * return ``-ENOMEM`` while invalid arguments result in ``-EINVAL``.
 *
 * @param heap Destination heap to initialise.
 * @param element_size Size in bytes of each element; must be non-zero.
 * @param capacity Number of elements to reserve on construction.
 * @param compare Comparator describing the ordering; must not be ``NULL``.
 * @return ``0`` on success, ``-EINVAL`` on invalid arguments, or ``-ENOMEM``
 *         when memory cannot be allocated.
 * @signature int ac_heap_with_capacity(ac_heap *heap, size_t element_size,
 *                                      size_t capacity, ac_compare_fn compare)
 */
int ac_heap_with_capacity(
    ac_heap *heap,
    size_t element_size,
    size_t capacity,
    ac_compare_fn compare
);

/**
 * @brief Build a heap from ``count`` elements copied from ``source``.
 *
 * The function performs a bottom-up heapify so that the resulting structure
 * matches the Python constructor that accepts an initial sequence.  The data is
 * copied into owned storage; ``source`` may be ``NULL`` only when
 * ``count == 0``.
 *
 * @param heap Destination heap to initialise.
 * @param element_size Size of each element in bytes.
 * @param source Pointer to the input array (nullable when ``count`` is zero).
 * @param count Number of elements in ``source``.
 * @param compare Comparator used to maintain ordering.
 * @return ``0`` on success, ``-EINVAL`` on invalid arguments, or ``-ENOMEM``
 *         when allocation fails.
 * @signature int ac_heap_from_array(ac_heap *heap, size_t element_size,
 *                                   const void *source, size_t count,
 *                                   ac_compare_fn compare)
 */
int ac_heap_from_array(
    ac_heap *heap,
    size_t element_size,
    const void *source,
    size_t count,
    ac_compare_fn compare
);

/**
 * @brief Release the resources owned by ``heap`` and reset its fields.
 *
 * Safe to call with ``NULL``; mirrors the destructor semantics of the Python
 * implementation by freeing storage and clearing size/capacity to zero.
 *
 * @param heap Pointer to an initialised heap (nullable).
 * @signature void ac_heap_destroy(ac_heap *heap)
 */
void ac_heap_destroy(ac_heap *heap);

/**
 * @brief Return the number of stored elements.
 *
 * Equivalent to the Python ``__len__`` helper.
 *
 * @param heap Pointer to an initialised heap (nullable).
 * @return Number of elements currently stored.
 * @signature size_t ac_heap_size(const ac_heap *heap)
 */
size_t ac_heap_size(const ac_heap *heap);

/**
 * @brief Report the allocated capacity in elements.
 *
 * @param heap Pointer to an initialised heap (nullable).
 * @return Count of slots available before reallocation.
 * @signature size_t ac_heap_capacity(const ac_heap *heap)
 */
size_t ac_heap_capacity(const ac_heap *heap);

/**
 * @brief Determine whether the heap contains zero elements.
 *
 * @param heap Pointer to an initialised heap (nullable).
 * @return ``true`` when empty, otherwise ``false``.
 * @signature bool ac_heap_empty(const ac_heap *heap)
 */
bool ac_heap_empty(const ac_heap *heap);

/**
 * @brief Ensure the heap can store at least ``new_capacity`` elements.
 *
 * Mirrors ``Vector.reserve`` in the Python translation.  Existing elements are
 * preserved; shrinking is unsupported here to keep the API minimal.
 *
 * @param heap Pointer to an initialised heap.
 * @param new_capacity Desired minimum capacity.
 * @return ``0`` on success, ``-EINVAL`` on invalid arguments, or ``-ENOMEM``
 *         when allocation fails.
 * @signature int ac_heap_reserve(ac_heap *heap, size_t new_capacity)
 */
int ac_heap_reserve(ac_heap *heap, size_t new_capacity);

/**
 * @brief Insert ``element`` into the heap while maintaining ordering.
 *
 * Equivalent to ``Heap.insert`` / ``append`` in the Python code.  The value is
 * copied into owned storage and sifted upwards until the heap property is
 * restored.  The function grows the allocation on demand.
 *
 * @param heap Pointer to an initialised heap.
 * @param element Address of the value to insert; must not be ``NULL``.
 * @return ``0`` on success, ``-EINVAL`` on invalid arguments, or ``-ENOMEM``
 *         when the heap cannot grow.
 * @signature int ac_heap_push(ac_heap *heap, const void *element)
 */
int ac_heap_push(ac_heap *heap, const void *element);

/**
 * @brief Copy the smallest element into ``out_element`` without removing it.
 *
 * The function fails with ``-ENOENT`` when the heap is empty or ``heap`` /
 * ``out_element`` is ``NULL``.
 *
 * @param heap Pointer to an initialised heap.
 * @param out_element Destination buffer for the peeked value.
 * @return ``0`` on success or ``-ENOENT`` when empty/invalid.
 * @signature int ac_heap_peek(const ac_heap *heap, void *out_element)
 */
int ac_heap_peek(const ac_heap *heap, void *out_element);

/**
 * @brief Remove the smallest element, copying it into ``out_element``.
 *
 * Mirrors ``Heap.remove_min`` / ``erase`` in the Python translation.  When the
 * heap is empty the function returns ``-ENOENT`` and leaves ``out_element``
 * untouched.
 *
 * @param heap Pointer to an initialised heap.
 * @param out_element Optional destination buffer for the removed value.
 * @return ``0`` on success or ``-ENOENT`` when the heap is empty or inputs are
 *         invalid.
 * @signature int ac_heap_pop(ac_heap *heap, void *out_element)
 */
int ac_heap_pop(ac_heap *heap, void *out_element);

/**
 * @brief Clear all elements without releasing the allocation.
 *
 * Resets the logical size to zero, allowing the heap to be reused without
 * paying another allocation cost.
 *
 * @param heap Pointer to an initialised heap (nullable).
 * @signature void ac_heap_clear(ac_heap *heap)
 */
void ac_heap_clear(ac_heap *heap);

/**
 * @brief Compute the height of the implicit binary tree.
 *
 * Equivalent to ``Heap.height`` in Python: returns the number of levels in the
 * tree representation, derived by repeatedly halving the current size.
 *
 * @param heap Pointer to an initialised heap (nullable).
 * @return Height of the heap measured in levels; zero when ``heap`` is
 * ``NULL``.
 * @signature size_t ac_heap_height(const ac_heap *heap)
 */
size_t ac_heap_height(const ac_heap *heap);

#ifdef __cplusplus
}
#endif

#endif
