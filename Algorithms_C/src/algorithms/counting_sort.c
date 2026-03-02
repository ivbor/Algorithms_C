#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

void ac_counting_sort_int(
    int *data,
    size_t size,
    int min_value,
    int max_value
) {
    if (data == NULL || size == 0) {
        return;
    }

    if (min_value > max_value) {
        int tmp = min_value;
        min_value = max_value;
        max_value = tmp;
    }

    size_t range = (size_t)(max_value - min_value + 1);
    size_t *counts = (size_t *)calloc(range, sizeof(size_t));
    if (counts == NULL) {
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        counts[data[i] - min_value]++;
    }

    size_t index = 0;
    for (size_t value = 0; value < range; ++value) {
        for (size_t repeat = 0; repeat < counts[value]; ++repeat) {
            data[index++] = (int)(value + min_value);
        }
    }

    free(counts);
}

void ac_count_sort_int_auto(int *data, size_t size) {
    if (data == NULL || size == 0U) {
        return;
    }

    int min_value = data[0];
    int max_value = data[0];
    for (size_t i = 1; i < size; ++i) {
        if (data[i] < min_value) {
            min_value = data[i];
        }
        if (data[i] > max_value) {
            max_value = data[i];
        }
    }

    ac_counting_sort_int(data, size, min_value, max_value);
}
