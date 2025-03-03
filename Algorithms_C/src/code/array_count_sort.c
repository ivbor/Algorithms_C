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

// Translate these tests from python to C, AI!
import random

from Algorithms_Python.array_count_sort import array_count_sort


def test_array_count_sort_case_one_elt_with_huge_variation():
    array_with_1_dim = [[random.randint(-10000, 10000) for _ in range(100)]]
    assert array_count_sort(array_with_1_dim) == \
        sorted(array_with_1_dim), \
        'optimization works wrong'


def test_array_count_sort_case_one_elt_in_2_dim():
    array_with_1_dim = [[random.randint(-100, 100)]
                        for _ in range(100)]
    assert array_count_sort(array_with_1_dim) == \
        sorted(array_with_1_dim), \
        'array_count_sort does not sort 1 dim arrays'


def test_array_count_sort_case_one_elt_in_2_dim_some_empty():
    array_with_1_dim = [[random.choice(
        (random.randint(-100, 100),))] for _ in range(100)]
    assert array_count_sort(array_with_1_dim) == \
        sorted(array_with_1_dim), \
        'array_count_sort does not sort 1 dim arrays with free places'


def test_array_count_sort_case_many_elts_no_key():
    array_with_2_dim = [[random.randint(-100, 100)
                         for _ in range(100)]
                        for _ in range(100)]
    assert array_count_sort(array_with_2_dim) == \
        sorted(array_with_2_dim, key=lambda x: x[0]), \
        'array_count_sort does not sort 2 dim arrays'


def test_array_count_sort_case_many_elts_no_key_some_empty():
    array_with_2_dim = [[random.choice((
        (random.randint(-100, 100),))) for _ in range(100)]
        for _ in range(100)]
    assert array_count_sort(array_with_2_dim) == \
        sorted(array_with_2_dim, key=lambda x: x[0]), \
        'array_count_sort does not sort 2 dim arrays with free places'


def test_array_count_sort_case_many_elts_with_key_some_empty():
    array_with_2_dim = [[random.choice((
        (random.randint(-100, 100),))) for _ in range(100)]
        for _ in range(100)]
    assert array_count_sort(array_with_2_dim, key=9) == \
        sorted(array_with_2_dim, key=lambda x: x[9]), \
        'array_count_sort does not sort 2 dim arrays with free places'

