#ifndef ALGORITHMS_C_ALGORITHMS_BINARY_SEARCH_H
#define ALGORITHMS_C_ALGORITHMS_BINARY_SEARCH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file binary_search.h
 * @brief Generic binary-search primitives used across translated algorithms.
 *
 * The Python repository exposes direct search helpers plus lower/upper bound
 * variants for range queries. The C translation keeps the same conceptual API
 * while using function pointers to compare opaque elements.
 */

/** Comparator signature used by binary search and sorting helpers. */
typedef int (*ac_compare_fn)(const void *lhs, const void *rhs);

/**
 * @brief Find an exact match for ``target`` in sorted storage.
 *
 * @param data Pointer to the first element in a sorted contiguous array.
 * @param size Number of elements in ``data``.
 * @param element_size Size in bytes of each element.
 * @param target Pointer to the element to locate.
 * @param compare Comparator returning negative/zero/positive ordering.
 * @return Zero-based index when found; ``-1`` when absent or inputs are
 *         invalid.
 * @signature ptrdiff_t ac_binary_search(const void *data, size_t size,
 *                                       size_t element_size,
 *                                       const void *target,
 *                                       ac_compare_fn compare)
 */
ptrdiff_t ac_binary_search(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
);

/**
 * @brief Return the first position whose value is not less than ``target``.
 *
 * Equivalent to Python's ``bisect_left`` and C++ ``lower_bound``.
 *
 * @param data Pointer to sorted storage.
 * @param size Number of elements in ``data``.
 * @param element_size Size of each element in bytes.
 * @param target Value used for boundary lookup.
 * @param compare Comparator defining array order.
 * @return Insertion index in range ``[0, size]``. Returns ``size`` on invalid
 *         input to stay fail-safe.
 * @signature size_t ac_lower_bound(const void *data, size_t size,
 *                                  size_t element_size,
 *                                  const void *target,
 *                                  ac_compare_fn compare)
 */
size_t ac_lower_bound(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
);

/**
 * @brief Return the first position whose value is greater than ``target``.
 *
 * Equivalent to Python's ``bisect_right`` and C++ ``upper_bound``.
 *
 * @param data Pointer to sorted storage.
 * @param size Number of elements in ``data``.
 * @param element_size Size of each element in bytes.
 * @param target Value used for boundary lookup.
 * @param compare Comparator defining array order.
 * @return Insertion index in range ``[0, size]`` after any equal elements.
 *         Returns ``size`` on invalid input.
 * @signature size_t ac_upper_bound(const void *data, size_t size,
 *                                  size_t element_size,
 *                                  const void *target,
 *                                  ac_compare_fn compare)
 */
size_t ac_upper_bound(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
);

/**
 * @brief Integer binary-search compatibility helper from ``bin_search.py``.
 *
 * Mirrors the Python interface that returns a boolean-like answer and supports
 * both recursive and non-recursive implementations.
 *
 * @param data Sorted integer array.
 * @param size Number of elements in ``data``.
 * @param value Value to search for.
 * @param no_recursion When true, use iterative mode; otherwise recursive mode.
 * @return ``1`` when found, otherwise ``0``.
 * @signature int ac_bin_search_int(const int *data, size_t size, int value,
 *                                  int no_recursion)
 */
int ac_bin_search_int(
    const int *data,
    size_t size,
    int value,
    int no_recursion
);

/**
 * @brief Convenience comparator for ``int`` values.
 *
 * @param lhs Pointer to first ``int``.
 * @param rhs Pointer to second ``int``.
 * @return Negative when ``*lhs < *rhs``, zero when equal, positive otherwise.
 * @signature int ac_compare_int(const void *lhs, const void *rhs)
 */
int ac_compare_int(const void *lhs, const void *rhs);

#ifdef __cplusplus
}
#endif

#endif
