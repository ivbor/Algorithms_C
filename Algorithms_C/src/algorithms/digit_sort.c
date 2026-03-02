#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

void ac_digit_sort_int(int *data, size_t size, int base) {
    if (data == NULL || size < 2U || base < 2) {
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

    uint64_t max_key = (uint64_t)((int64_t)max_value - (int64_t)min_value);
    if (max_key == 0U) {
        return;
    }

    int *output = (int *)malloc(size * sizeof(int));
    uint64_t *keys = (uint64_t *)malloc(size * sizeof(uint64_t));
    uint64_t *keys_output = (uint64_t *)malloc(size * sizeof(uint64_t));
    size_t *count = (size_t *)calloc((size_t)base, sizeof(size_t));

    if (output == NULL || keys == NULL || keys_output == NULL ||
        count == NULL) {
        free(output);
        free(keys);
        free(keys_output);
        free(count);
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        keys[i] = (uint64_t)((int64_t)data[i] - (int64_t)min_value);
    }

    uint64_t exp = 1U;
    while (max_key / exp > 0U) {
        memset(count, 0, (size_t)base * sizeof(size_t));

        for (size_t i = 0; i < size; ++i) {
            size_t digit = (size_t)((keys[i] / exp) % (uint64_t)base);
            count[digit]++;
        }

        for (size_t i = 1; i < (size_t)base; ++i) {
            count[i] += count[i - 1U];
        }

        for (size_t index = size; index > 0; --index) {
            size_t i = index - 1U;
            size_t digit = (size_t)((keys[i] / exp) % (uint64_t)base);
            size_t position = --count[digit];
            output[position] = data[i];
            keys_output[position] = keys[i];
        }

        memcpy(data, output, size * sizeof(int));
        memcpy(keys, keys_output, size * sizeof(uint64_t));

        if (exp > UINT64_MAX / (uint64_t)base) {
            break;
        }
        exp *= (uint64_t)base;
    }

    free(output);
    free(keys);
    free(keys_output);
    free(count);
}

void ac_digit_sort_opt_int(int *data, size_t size, int base) {
    if (data == NULL || size < 2U || base < 2) {
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

    uint64_t max_key = (uint64_t)((int64_t)max_value - (int64_t)min_value);
    if (max_key == 0U) {
        return;
    }

    uint64_t *keys = (uint64_t *)malloc(size * sizeof(uint64_t));
    uint64_t *keys_output = (uint64_t *)malloc(size * sizeof(uint64_t));
    int *output = (int *)malloc(size * sizeof(int));
    size_t *count = (size_t *)calloc((size_t)base, sizeof(size_t));
    if (keys == NULL || keys_output == NULL || output == NULL ||
        count == NULL) {
        free(keys);
        free(keys_output);
        free(output);
        free(count);
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        keys[i] = (uint64_t)((int64_t)data[i] - (int64_t)min_value);
    }

    uint64_t digit_place = 1U;
    while (max_key / digit_place > 0U) {
        memset(count, 0, (size_t)base * sizeof(size_t));

        for (size_t i = 0; i < size; ++i) {
            size_t digit = (size_t)((keys[i] / digit_place) % (uint64_t)base);
            count[digit]++;
        }

        for (size_t i = 1; i < (size_t)base; ++i) {
            count[i] += count[i - 1U];
        }

        for (size_t idx = size; idx > 0; --idx) {
            size_t i = idx - 1U;
            size_t digit = (size_t)((keys[i] / digit_place) % (uint64_t)base);
            size_t pos = --count[digit];
            output[pos] = data[i];
            keys_output[pos] = keys[i];
        }

        memcpy(data, output, size * sizeof(int));
        memcpy(keys, keys_output, size * sizeof(uint64_t));

        if (digit_place > UINT64_MAX / (uint64_t)base) {
            break;
        }
        digit_place *= (uint64_t)base;
    }

    free(keys);
    free(keys_output);
    free(output);
    free(count);
}
