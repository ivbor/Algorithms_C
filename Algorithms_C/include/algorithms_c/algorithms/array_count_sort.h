#ifndef ALGORITHMS_C_ALGORITHMS_ARRAY_COUNT_SORT_H
#define ALGORITHMS_C_ALGORITHMS_ARRAY_COUNT_SORT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file array_count_sort.h
 * @brief Counting-sort helper for two-dimensional integer arrays.
 *
 * The Python project ships an educational routine that sorts a matrix by a
 * selected key column while keeping rows intact. This header mirrors that
 * helper and documents both behaviour and signature inline so the translation
 * remains self-contained for readers who study the API before opening sources.
 */

/**
 * @brief Stable-sort matrix rows by values in column ``key``.
 *
 * The algorithm performs a counting-sort pass over the selected column,
 * preserving relative order for rows with equal keys. The matrix is expected
 * to contain non-negative values inside the key column because counts are used
 * directly as indexes. Invalid inputs are treated as no-ops, matching the
 * defensive style used across the translated repository.
 *
 * @param arr Matrix represented as an array of row pointers.
 * @param rows Number of rows in ``arr``.
 * @param cols Number of columns in each row.
 * @param key Zero-based column index used as the sorting key.
 * @signature void ac_array_count_sort(int **arr, size_t rows, size_t cols,
 *                                     size_t key)
 */
void ac_array_count_sort(int **arr, size_t rows, size_t cols, size_t key);

#ifdef __cplusplus
}
#endif

#endif
