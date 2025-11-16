#ifndef ALGORITHMS_C_STRUCTURES_MATRIX_VIEW_H
#define ALGORITHMS_C_STRUCTURES_MATRIX_VIEW_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file matrix_view.h
 * @brief Helper utilities for rendering integer matrices in tabular form.
 *
 * The Python repository exposes a ``print_matrix`` helper with extensive
 * commentary on formatting decisions.  The C version mirrors that spirit by
 * documenting both the short and long descriptions inline.
 */

/**
 * @brief Render a matrix of integers to standard output.
 *
 * The function prints rows and columns with consistent alignment and optionally
 * emits header indexes for additional clarity—matching the behaviour of the
 * Python helper.
 *
 * @param matrix Two-dimensional array expressed as an array of row pointers.
 * @param rows Number of rows in ``matrix``.
 * @param columns Number of columns in each row.
 * @param print_indexes Non-zero to print row/column indexes alongside values.
 * @signature void ac_print_matrix(int **matrix, size_t rows, size_t columns,
 *                                 int print_indexes)
 */
void ac_print_matrix(
    int **matrix,
    size_t rows,
    size_t columns,
    int print_indexes
);

#ifdef __cplusplus
}
#endif

#endif
