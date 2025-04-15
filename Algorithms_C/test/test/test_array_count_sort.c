#ifndef NO_STARS
#include "../../include/macros.h"
#endif

#ifndef minunit
#include "../../include/minunit.h"
#endif

#ifndef logger
#include "../../include/logger.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../src/headers/array_count_sort.h"
#include "../../src/headers/compare.h"
#include "../../src/headers/matrix_view.h"

int key = 0;

int comp_func(const void *a, const void *b) {
    if (deref((int **)a)[key] > deref((int **)b)[key])
        return 1;
    else if (deref((int **)a) < deref((int **)b))
        return -1;
    else
        return 0;
};

char *run_array_count_sort_test_case(
    int rows,
    int cols,
    int key_col,
    int min_val,
    int max_val,
    const char *assert_message,
    float empty_ratio
) {
    int **array = (int **)malloc(rows * sizeof(int *));
    int **copy = (int **)malloc(rows * sizeof(int *));

    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        if ((float)rand() / RAND_MAX < empty_ratio) {
            array[i] = NULL;
            copy[i] = NULL;
            continue;
        }

        array[i] = (int *)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            array[i][j] = rand() % (max_val - min_val + 1) + min_val;
        }
        copy[i] = array[i];
    }

    log_message(LOG_DEBUG, "Original array:\n");
    printMatrix(array, rows, cols, 1);

    array_count_sort(array, rows, cols, key_col);

    log_message(LOG_DEBUG, "Sorted array:\n");
    printMatrix(array, rows, cols, 1);

    qsort(copy, rows, sizeof(int *), comp_func);

    log_message(LOG_DEBUG, "Copied array after qsort:\n");
    printMatrix(copy, rows, cols, 1);

    run_assert(assert_message, compare2dimArraysint(array, copy, rows, cols));

    for (int i = 0; i < rows; i++) {
        free(copy[i]);
    }
    free(array);
    free(copy);
    return 0;
}

char *test_array_count_sort_case_one_elt_with_huge_variation() {
    return run_array_count_sort_test_case(
        10, 10, 0, -10000, 10000,
        "Array with high value variance should be sorted.", 0
    );
}

char *test_array_count_sort_case_one_elt_in_2_dim() {
    return run_array_count_sort_test_case(
        100, 1, 0, -100, 100, "One-elt per row should be sorted.", 0
    );
}

char *test_array_count_sort_case_one_elt_in_2_dim_some_empty() {
    return run_array_count_sort_test_case(
        100, 1, 0, -100, 100, "One-elt with empty rows should be sorted.", 0.1f
    );
}

char *test_array_count_sort_case_many_elts_no_key() {
    return run_array_count_sort_test_case(
        100, 100, 0, -100, 100, "Many elements, no key, should be sorted.", 0
    );
}

char *test_array_count_sort_case_many_elts_no_key_some_empty() {
    return run_array_count_sort_test_case(
        100, 100, 0, -100, 100,
        "Many elements with empty rows, no key, should be sorted.", 0.1f
    );
}

char *test_array_count_sort_case_many_elts_with_key_some_empty() {
    return run_array_count_sort_test_case(
        100, 100, 9, -100, 100, "Many elements sorted by key column 9.", 0.1f
    );
}
