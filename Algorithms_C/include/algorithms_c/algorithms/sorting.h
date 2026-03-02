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
 * @brief Bubble sort with adjacent swaps and early-exit optimization.
 *
 * The helper mirrors the translated Python educational routine: repeated
 * adjacent comparisons move larger elements toward the array tail on each
 * pass. If a full pass performs no swaps, the routine stops early.
 *
 * @signature void ac_bubble_sort(void *data, size_t size,
 *                                size_t element_size,
 *                                ac_compare_fn compare)
 */
void ac_bubble_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);

/**
 * @brief Quadratic-time selection sort that minimizes swap count.
 *
 * The helper mirrors the translated Python implementation used for teaching:
 * repeatedly find the minimum in the unsorted suffix and swap it into the next
 * output slot. This algorithm is intentionally simple and useful for
 * algorithm-comparison exercises.
 *
 * @signature void ac_selection_sort(void *data, size_t size,
 *                                   size_t element_size,
 *                                   ac_compare_fn compare)
 */
void ac_selection_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);

/**
 * @brief Shell sort with shrinking-gap insertion passes.
 *
 * The helper mirrors the translated Python educational routine with a simple
 * gap sequence ``N/2, N/4, ..., 1``.
 *
 * @signature void ac_shell_sort(void *data, size_t size,
 *                               size_t element_size,
 *                               ac_compare_fn compare)
 */
void ac_shell_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
);

/**
 * @brief Comb sort with shrinking gap to reduce turtle inversions.
 *
 * Source file translated in this step:
 * ``Algorithms_Python/algorithms/sorting/comb_sort.py``.
 *
 * @signature void ac_comb_sort(void *data, size_t size,
 *                              size_t element_size,
 *                              ac_compare_fn compare)
 */
void ac_comb_sort(
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

/**
 * @brief Counting sort that infers min/max bounds from the input array.
 *
 * Mirrors ``Algorithms_Python/count_sort.py`` where bounds are computed from
 * the data before counting frequencies.
 *
 * @param data Array of integers to sort in ascending order.
 * @param size Number of elements in ``data``.
 * @signature void ac_count_sort_int_auto(int *data, size_t size)
 */
void ac_count_sort_int_auto(int *data, size_t size);

/**
 * @brief Radix-style digit sort for signed integer arrays.
 *
 * Mirrors ``Algorithms_Python/digit_sort.py`` by normalising values with the
 * minimum element and sorting digits from least significant to most
 * significant using stable counting passes.
 *
 * @param data Array of integers to sort in ascending order.
 * @param size Number of elements in ``data``.
 * @param base Radix base for digit extraction; values below 2 are ignored.
 * @signature void ac_digit_sort_int(int *data, size_t size, int base)
 */
void ac_digit_sort_int(int *data, size_t size, int base);

#ifdef __cplusplus
}
#endif

#endif
