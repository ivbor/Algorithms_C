#include "algorithms_c/structures/matrix_view.h"
#include <stdio.h>

/*
 * This implementation intentionally keeps logic straightforward so the helper
 * reads like the educational Python original. The function prints optional
 * axis indexes first, then emits every row using fixed-width formatting so
 * rectangular matrices remain visually aligned for debugging and lectures.
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

    /* Print column headers when callers request explicit coordinates. */
    if (print_indexes) {
        printf("    ");
        for (size_t col = 0; col < columns; ++col) {
            printf("%4zu ", col);
        }
        printf("\n");
    }

    /* Emit matrix data row by row, optionally prefixing each row index. */
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
