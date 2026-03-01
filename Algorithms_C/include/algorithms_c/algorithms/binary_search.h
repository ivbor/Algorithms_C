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
 * @brief Interpolation search for sorted integer arrays.
 *
 * This helper mirrors Python implementations of interpolation search used for
 * uniformly distributed numeric data. Unlike generic binary search it estimates
 * the probe position based on the target value and boundary values, which can
 * reduce comparisons on near-uniform inputs.
 *
 * @param data Pointer to a sorted ``int`` array in ascending order.
 * @param size Number of elements in ``data``.
 * @param target Integer value to locate.
 * @return Zero-based index of ``target`` when found, or ``-1`` when absent or
 *         arguments are invalid.
 * @signature ptrdiff_t ac_interpolation_search_int(const int *data,
 *                                                  size_t size,
 *                                                  int target)
 */
ptrdiff_t ac_interpolation_search_int(const int *data, size_t size, int target);


/**
 * @brief Jump search for sorted integer arrays.
 *
 * This helper mirrors the Python jump-search variant that advances in
 * ``sqrt(n)`` sized blocks and then performs a linear scan within the located
 * block. It is useful as another educational search strategy between linear and
 * binary search.
 *
 * @param data Pointer to a sorted ``int`` array in ascending order.
 * @param size Number of elements in ``data``.
 * @param target Integer value to locate.
 * @return Zero-based index of ``target`` when found, or ``-1`` when absent or
 *         arguments are invalid.
 * @signature ptrdiff_t ac_jump_search_int(const int *data, size_t size,
 *                                         int target)
 */
ptrdiff_t ac_jump_search_int(const int *data, size_t size, int target);

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
