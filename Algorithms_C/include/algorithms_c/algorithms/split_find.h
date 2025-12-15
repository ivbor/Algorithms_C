#ifndef ALGORITHMS_C_ALGORITHMS_SPLIT_FIND_H
#define ALGORITHMS_C_ALGORITHMS_SPLIT_FIND_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select the element that would appear at a given index after sorting.
 *
 * The helper mirrors the Python ``split_find`` routine: it walks the array
 * using a randomized pivot and partial partitioning so that only the necessary
 * region is explored.  The function never fully sorts the data, which keeps it
 * a lightweight building block when you only need the k-th order statistic.
 * Invalid inputs are rejected defensively to keep undefined behaviour out of
 * the educational translation.
 *
 * @param array Pointer to the buffer of ``double`` values to inspect; must not
 *     be ``NULL``.
 * @param size Number of valid elements stored in ``array``.
 * @param index Zero-based position to retrieve as if the array were sorted in
 *     ascending order.
 * @param out_value Output pointer that receives the element; must not be
 *     ``NULL``.
 * @return ``0`` on success, ``-1`` when inputs are invalid or ``index`` lies
 *     outside the array bounds.
 * @signature int ac_split_find(double *array, size_t size, size_t index,
 *                              double *out_value)
 */
int ac_split_find(double *array, size_t size, size_t index, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
