#ifndef ALGORITHMS_C_ALGORITHMS_SORTING_H
#define ALGORITHMS_C_ALGORITHMS_SORTING_H

#include <stddef.h>
#include "algorithms_c/algorithms/binary_search.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sorting.h
 * @brief Generic sorting algorithms translated from Algorithms_Python.
 *
 * Each helper accepts raw contiguous storage and an ``ac_compare_fn`` callback
 * so that the same implementation can sort any trivially copyable type.
 */

/**
 * @brief Stable insertion sort for small arrays or nearly sorted inputs.
 *
 * @signature void ac_insertion_sort(void *data, size_t size,
 *                                   size_t element_size,
 *                                   ac_compare_fn compare)
 */
void ac_insertion_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);

/**
 * @brief Stable divide-and-conquer merge sort.
 *
 * @signature void ac_merge_sort(void *data, size_t size, size_t element_size,
 *                               ac_compare_fn compare)
 */
void ac_merge_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);

/**
 * @brief In-place quick sort with recursive partitioning.
 *
 * @signature void ac_quick_sort(void *data, size_t size, size_t element_size,
 *                               ac_compare_fn compare)
 */
void ac_quick_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);


/**
 * @brief Heap sort implemented via the documented ``ac_heap`` container.
 *
 * The helper mirrors the Python ``heap_sort`` routine: all elements are first
 * inserted into a min-heap and then extracted in ascending order back into the
 * original buffer. The comparator defines the ordering and must return a
 * negative value when the first argument is "less" than the second. When
 * inputs are invalid the function becomes a no-op, matching the defensive style
 * used across the translated sorting helpers.
 *
 * @param data Pointer to the array to sort; must not be ``NULL``.
 * @param size Number of elements contained in ``data``.
 * @param element_size Size in bytes of each element in ``data``.
 * @param compare Comparator that defines the ordering of elements.
 * @signature void ac_heap_sort(void *data, size_t size, size_t element_size,
 *                              ac_compare_fn compare)
 */
void ac_heap_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);

/**
 * @brief Counting sort specialised for integer arrays within known bounds.
 *
 * @param data Array of integers to sort in ascending order.
 * @param size Number of elements in ``data``.
 * @param min_value Minimum value expected in ``data``.
 * @param max_value Maximum value expected in ``data``.
 * @signature void ac_counting_sort_int(int *data, size_t size, int min_value,
 *                                      int max_value)
 */
void ac_counting_sort_int(int *data, size_t size, int min_value, int max_value);

#ifdef __cplusplus
}
#endif

#endif
