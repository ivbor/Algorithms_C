#ifndef ALGORITHMS_C_STRUCTURES_MATRIX_VIEW_H
#define ALGORITHMS_C_STRUCTURES_MATRIX_VIEW_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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
