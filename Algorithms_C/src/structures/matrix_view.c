#include "algorithms_c/structures/matrix_view.h"
#include <stdio.h>

void ac_print_matrix(
    int **matrix,
    size_t rows,
    size_t columns,
    int print_indexes
) {
    if (matrix == NULL) {
        return;
    }

    if (print_indexes) {
        printf("    ");
        for (size_t col = 0; col < columns; ++col) {
            printf("%4zu ", col);
        }
        printf("\n");
    }

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
