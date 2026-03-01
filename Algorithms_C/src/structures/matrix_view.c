#include "algorithms_c/structures/matrix_view.h"
#include <stdio.h>

/*
 * This implementation intentionally remains compact while preserving the
 * educational flavor of the Python helper.  Instead of introducing formatter
 * abstractions, the function keeps explicit loops for headers and rows so
 * students can directly map printed output to matrix coordinates.
 */
void ac_print_matrix(
    int **matrix,
    size_t rows,
    size_t columns,
    int print_indexes
) {
    if (matrix == NULL) {
        return;
    }

    /*
     * Optional column index header.  The leading padding reserves space for row
     * labels so data columns align regardless of whether indexes are enabled.
     */
    if (print_indexes) {
        printf("    ");
        for (size_t col = 0; col < columns; ++col) {
            printf("%4zu ", col);
        }
        printf("\n");
    }

    /*
     * Emit matrix content row-by-row.  Every numeric cell uses a fixed width
     * of four characters plus a trailing space, keeping columns easy to scan in
     * test logs and manual debugging sessions.
     */
    for (size_t row = 0; row < rows; ++row) {
        if (print_indexes) {
            printf("%4zu ", row);
        }
        for (size_t col = 0; col < columns; ++col) {
            printf("%4d ", matrix[row][col]);
        }
        printf("\n");
    }
}
