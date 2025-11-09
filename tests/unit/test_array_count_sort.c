#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "algorithms_c/algorithms/array_count_sort.h"
#include "algorithms_c/utils/minunit.h"

static int **allocate_matrix(size_t rows, size_t cols) {
    int **matrix = (int **)calloc(rows, sizeof(int *));
    if (matrix == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < rows; ++i) {
        matrix[i] = (int *)calloc(cols, sizeof(int));
        if (matrix[i] == NULL) {
            for (size_t j = 0; j < i; ++j) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

static void free_matrix(int **matrix, size_t rows) {
    if (matrix == NULL) {
        return;
    }
    for (size_t i = 0; i < rows; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

static size_t g_key_column = 0;

static int compare_rows(const void *lhs, const void *rhs) {
    const int *const *left = (const int *const *)lhs;
    const int *const *right = (const int *const *)rhs;
    size_t key = g_key_column;
    if ((*left)[key] < (*right)[key]) {
        return -1;
    }
    if ((*left)[key] > (*right)[key]) {
        return 1;
    }
    return 0;
}

static void run_randomized_case(size_t rows, size_t cols, size_t key) {
    int **data = allocate_matrix(rows, cols);
    MU_ASSERT(data != NULL);

    int **copy = allocate_matrix(rows, cols);
    MU_ASSERT(copy != NULL);

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            int value = rand() % 2001 - 1000;
            data[i][j] = value;
            copy[i][j] = value;
        }
    }

    ac_array_count_sort(data, rows, cols, key);

    g_key_column = key;
    qsort(copy, rows, sizeof(int *), compare_rows);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            MU_ASSERT(data[i][j] == copy[i][j]);
        }
    }

    free_matrix(data, rows);
    free_matrix(copy, rows);
}

static void test_small_matrix_sorted_by_first_column(void) {
    int *data[] = {(int[]){3, 1}, (int[]){1, 4}, (int[]){2, 5}};

    ac_array_count_sort(data, 3, 2, 0);

    int expected[][2] = {{1, 4}, {2, 5}, {3, 1}};
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            MU_ASSERT(data[i][j] == expected[i][j]);
        }
    }
}

static void test_randomized_matrix(void) {
    run_randomized_case(64, 8, 3);
}

void register_array_count_sort_tests(void) {
    run_test(test_small_matrix_sorted_by_first_column);
    run_test(test_randomized_matrix);
}
