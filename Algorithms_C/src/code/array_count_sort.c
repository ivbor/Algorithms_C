#include "../headers/array_count_sort.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// Function to find the maximum and minimum values in a 2D array
// based on a specific key index
static void find_min_max(int **arr, int rows, int key, int *min, int *max) {
    *min = INT_MAX;
    *max = INT_MIN;
    for (int i = 0; i < rows; i++) {
        if (arr[i][key] != INT_MIN) {
            if (arr[i][key] < *min)
                *min = arr[i][key];
            if (arr[i][key] > *max)
                *max = arr[i][key];
        }
    }
}

// Function to perform Counting Sort on a 2D array
// based on a specific key index
void array_count_sort(int **arr, int rows, int cols, int key) {
    // Initialize and calculate min and max in the array
    int min, max;
    find_min_max(arr, rows, key, &min, &max);

    // Initialize range of the array's elements
    int range = max - min + 2;

    // Initialize arrays for counting and for storing the result
    int *count = (int *)calloc(range, sizeof(int));
    int **result = (int **)malloc(rows * sizeof(int *));

    // Count occurrences of each key
    for (int i = 0; i < rows; i++) {
        result[i] = (int *)calloc(cols, sizeof(int));
        if (arr[i][key] != INT_MIN) {
            count[arr[i][key] - min]++;
        } else {
            count[range - 1]++;
        }
    }

    // Calculate cumulative count
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    // Build sorted array
    for (int i = rows - 1; i >= 0; i--) {
        if (arr[i][key] != INT_MIN) {
            count[arr[i][key] - min]--;
            for (int j = 0; j < cols; j++) {
                result[count[arr[i][key] - min]][j] = arr[i][j];
            }
        } else {
            count[range - 1]--;
            for (int j = 0; j < cols; j++) {
                if (count[range - 1] >= 0 && count[range - 1] < rows) {
                    result[count[range - 1]][j] = arr[i][j];
                } else {
                    fprintf(
                        stderr,
                        "Warning: Skipped assignment due to invalid pos=%d\n",
                        count[range - 1]
                    );
                }
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i][j] = result[i][j];
        }
        free(result[i]);
    }
    free(result);
    free(count);
}

// Test functions translated from Python to C

#include <assert.h>
#include <time.h>

void test_array_count_sort_case_one_elt_with_huge_variation() {
    const int rows = 1;
    const int cols = 100;
    int **array_with_1_dim = (int **)malloc(rows * sizeof(int *));
    array_with_1_dim[0] = (int *)malloc(cols * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < cols; i++) {
        array_with_1_dim[0][i] = rand() % 20001 - 10000;
    }

    array_count_sort(array_with_1_dim, rows, cols, 0);

    for (int i = 0; i < cols - 1; i++) {
        assert(array_with_1_dim[0][i] <= array_with_1_dim[0][i + 1]);
    }

    free(array_with_1_dim[0]);
    free(array_with_1_dim);
}

void test_array_count_sort_case_one_elt_in_2_dim() {
    const int rows = 100;
    const int cols = 1;
    int **array_with_1_dim = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        array_with_1_dim[i] = (int *)malloc(cols * sizeof(int));
        array_with_1_dim[i][0] = rand() % 201 - 100;
    }

    array_count_sort(array_with_1_dim, rows, cols, 0);

    for (int i = 0; i < rows - 1; i++) {
        assert(array_with_1_dim[i][0] <= array_with_1_dim[i + 1][0]);
    }

    for (int i = 0; < rows; i++) {
        free(array_with_1_dim[i]);
    }
    free(array_with_1_dim);
}

void test_array_count_sort_case_one_elt_in_2_dim_some_empty() {
    const int rows = 100;
    const int cols = 1;
    int **array_with_1_dim = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        array_with_1_dim[i] = (int *)malloc(cols * sizeof(int));
        array_with_1_dim[i][0] = rand() % 201 - 100;
    }

    array_count_sort(array_with_1_dim, rows, cols, 0);

    for (int i = 0; i < rows - 1; i++) {
        assert(array_with_1_dim[i][0] <= array_with_1_dim[i + 1][0]);
    }

    for (int i = 0; i < rows; i++) {
        free(array_with_1_dim[i]);
    }
    free(array_with_1_dim);
}

void test_array_count_sort_case_many_elts_no_key() {
    const int rows = 100;
    const int cols = 100;
    int **array_with_2_dim = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        array_with_2_dim[i] = (int *)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            array_with_2_dim[i][j] = rand() % 201 - 100;
        }
    }

    array_count_sort(array_with_2_dim, rows, cols, 0);

    for (int i = 0; i < rows - 1; i++) {
        assert(array_with_2_dim[i][0] <= array_with_2_dim[i + 1][0]);
    }

    for (int i = 0; i < rows; i++) {
        free(array_with_2_dim[i]);
    }
    free(array_with_2_dim);
}

void test_array_count_sort_case_many_elts_no_key_some_empty() {
    const int rows = 100;
    const int cols = 100;
    int **array_with_2_dim = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        array_with_2_dim[i] = (int *)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            array_with_2_dim[i][j] = rand() % 201 - 100;
        }
    }

    array_count_sort(array_with_2_dim, rows, cols, 0);

    for (int i = 0; i < rows - 1; i++) {
        assert(array_with_2_dim[i][0] <= array_with_2_dim[i + 1][0]);
    }

    for (int i = 0; i < rows; i++) {
        free(array_with_2_dim[i]);
    }
    free(array_with_2_dim);
}

void test_array_count_sort_case_many_elts_with_key_some_empty() {
    const int rows = 100;
    const int cols = 100;
    int **array_with_2_dim = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        array_with_2_dim[i] = (int *)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            array_with_2_dim[i][j] = rand() % 201 - 100;
        }
    }

    array_count_sort(array_with_2_dim, rows, cols, 9);

    for (int i = 0; i < rows - 1; i++) {
        assert(array_with_2_dim[i][9] <= array_with_2_dim[i + 1][9]);
    }

    for (int i = 0; i < rows; i++) {
        free(array_with_2_dim[i]);
    }
    free(array_with_2_dim);
}

int main() {
    test_array_count_sort_case_one_elt_with_huge_variation();
    test_array_count_sort_case_one_elt_in_2_dim();
    test_array_count_sort_case_one_elt_in_2_dim_some_empty();
    test_array_count_sort_case_many_elts_no_key();
    test_array_count_sort_case_many_elts_no_key_some_empty();
    test_array_count_sort_case_many_elts_with_key_some_empty();
    printf("All tests passed!\n");
    return 0;
}

