#ifndef ALGORITHMS_C_ALGORITHMS_COMPARE_H
#define ALGORITHMS_C_ALGORITHMS_COMPARE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file compare.h
 * @brief Equality helpers for arrays and matrices used by tests/examples.
 */

/**
 * @brief Compare two ``double`` arrays element-by-element.
 *
 * @param a First array.
 * @param b Second array.
 * @param n Number of elements to compare.
 * @return ``1`` when all elements are equal, ``0`` otherwise.
 * @signature int ac_compare_arrays_double(const double *a, const double *b,
 *                                         size_t n)
 */
int ac_compare_arrays_double(const double *a, const double *b, size_t n);

/**
 * @brief Compare two ``int`` arrays element-by-element.
 *
 * @param a First array.
 * @param b Second array.
 * @param n Number of elements to compare.
 * @return ``1`` when all elements are equal, ``0`` otherwise.
 * @signature int ac_compare_arrays_int(const int *a, const int *b, size_t n)
 */
int ac_compare_arrays_int(const int *a, const int *b, size_t n);

/**
 * @brief Compare two integer matrices element-by-element.
 *
 * @param a First matrix represented as row pointers.
 * @param b Second matrix represented as row pointers.
 * @param rows Number of rows.
 * @param cols Number of columns per row.
 * @return ``1`` when all entries match, ``0`` otherwise.
 * @signature int ac_compare_matrices_int(int **a, int **b, size_t rows,
 *                                        size_t cols)
 */
int ac_compare_matrices_int(int **a, int **b, size_t rows, size_t cols);

#ifdef __cplusplus
}
#endif

#endif
