#include "algorithms_c/algorithms/compare.h"
#include <math.h>
#include <stdio.h>

int ac_compare_arrays_double(const double *a, const double *b, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (fabs(a[i] - b[i]) >= 1e-10 * (fabs(a[i]) + fabs(b[i]))) {
            return 0;
        }
    }
    return 1;
}

int ac_compare_arrays_int(const int *a, const int *b, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

int ac_compare_matrices_int(int **a, int **b, size_t rows, size_t cols) {
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if (a[row][col] != b[row][col]) {
                fprintf(stderr, "Mismatch row=%zu col=%zu\n", row, col);
                return 0;
            }
        }
    }
    return 1;
}
