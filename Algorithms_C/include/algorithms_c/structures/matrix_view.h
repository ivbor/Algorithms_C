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
 * educational commentary on layout and readability.  The C translation mirrors
 * that style by documenting *why* each formatting decision exists, not only
 * what function is called.  The helper is intentionally small in code but rich
 * in prose so learners can quickly understand indexing, alignment, and
 * diagnostic intent.
 */

/**
 * @brief Render a matrix of integers to standard output.
 *
 * The function prints rows and columns using fixed-width cells so rectangular
 * data remains visually aligned, which makes debugging dynamic-programming
 * tables significantly easier.  When ``print_indexes`` is non-zero, a header
 * row and leading row labels are emitted to expose coordinates explicitly,
 * matching the pedagogical output style used by the Python source.
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
