#include "algorithms_c/algorithms/array_count_sort.h"
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Function to find the maximum and minimum values in a 2D array
// based on a specific key index
static void
find_min_max(int **arr, size_t rows, size_t key, int *min, int *max) {
    *min = INT_MAX;
    *max = INT_MIN;
    for (size_t i = 0; i < rows; ++i) {
        if (arr[i][key] != INT_MIN) {
            if (arr[i][key] < *min) {
                *min = arr[i][key];
            }
            if (arr[i][key] > *max) {
                *max = arr[i][key];
            }
        }
    }
}

// Function to perform Counting Sort on a 2D array
// based on a specific key index
void ac_array_count_sort(int **arr, size_t rows, size_t cols, size_t key) {
    // Initialize and calculate min and max in the array
    int min;
    int max;
    find_min_max(arr, rows, key, &min, &max);

    // Initialize range of the array's elements
    size_t range = (size_t)(max - min + 2);

    // Initialize arrays for counting and for storing the result
    size_t *count = (size_t *)calloc(range, sizeof(size_t));
    int **result = (int **)malloc(rows * sizeof(int *));

    // Count occurrences of each key
    for (size_t i = 0; i < rows; i++) {
        result[i] = (int *)calloc(cols, sizeof(int));
        if (arr[i][key] != INT_MIN) {
            count[arr[i][key] - min]++;
        } else {
            count[range - 1]++;
        }
    }

    // Calculate cumulative count
    for (size_t i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    // Build sorted array
    for (size_t index = rows; index > 0; index--) {
        size_t i = index - 1;
        if (arr[i][key] != INT_MIN) {
            size_t position = --count[arr[i][key] - min];
            for (size_t j = 0; j < cols; j++) {
                result[position][j] = arr[i][j];
            }
        } else {
            size_t position = --count[range - 1];
            for (size_t j = 0; j < cols; j++) {
                if (position < rows) {
                    result[position][j] = arr[i][j];
                }
            }
        }
    }
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            arr[i][j] = result[i][j];
        }
        free(result[i]);
    }
    free(result);
    free(count);
}
