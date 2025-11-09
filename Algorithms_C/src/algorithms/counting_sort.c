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
