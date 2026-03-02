#ifndef ALGORITHMS_C_ALGORITHMS_BOUNDS_H
#define ALGORITHMS_C_ALGORITHMS_BOUNDS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file bounds.h
 * @brief Integer lower/upper-bound helpers translated from bounds.py.
 *
 * The Python implementation focuses on sorted integer arrays and returns
 * boundaries for duplicate runs. This header provides equivalent helpers with
 * explicit validity checks and no-allocation behaviour.
 */

/**
 * @brief Find index of the first element equal to ``value`` in sorted array.
 *
 * @param array Sorted integer array.
 * @param size Number of elements in ``array``.
 * @param value Value to locate.
 * @return Index of first matching element, or ``-1`` when absent/invalid.
 * @signature ptrdiff_t ac_bounds_lower_int(const int *array, size_t size,
 *                                          int value)
 */
ptrdiff_t ac_bounds_lower_int(const int *array, size_t size, int value);

/**
 * @brief Find index of the last element equal to ``value`` in sorted array.
 *
 * @param array Sorted integer array.
 * @param size Number of elements in ``array``.
 * @param value Value to locate.
 * @return Index of last matching element, or ``-1`` when absent/invalid.
 * @signature ptrdiff_t ac_bounds_upper_int(const int *array, size_t size,
 *                                          int value)
 */
ptrdiff_t ac_bounds_upper_int(const int *array, size_t size, int value);

#ifdef __cplusplus
}
#endif

#endif
