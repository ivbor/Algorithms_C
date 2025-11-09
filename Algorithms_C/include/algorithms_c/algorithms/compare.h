#ifndef ALGORITHMS_C_ALGORITHMS_COMPARE_H
#define ALGORITHMS_C_ALGORITHMS_COMPARE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int ac_compare_arrays_double(const double *a, const double *b, size_t n);
int ac_compare_arrays_int(const int *a, const int *b, size_t n);
int ac_compare_matrices_int(int **a, int **b, size_t rows, size_t cols);

#ifdef __cplusplus
}
#endif

#endif
