#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

void ac_insertion_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    if (data == NULL || compare == NULL || element_size == 0) {
        return;
    }

    unsigned char *array = (unsigned char *)data;
    unsigned char *buffer = (unsigned char *)malloc(element_size);
    if (buffer == NULL) {
        return;
    }

    for (size_t i = 1; i < size; ++i) {
        memcpy(buffer, array + (i * element_size), element_size);
        size_t j = i;
        while (j > 0) {
            unsigned char *prev = array + ((j - 1) * element_size);
            if (compare(prev, buffer) <= 0) {
                break;
            }
            memcpy(array + (j * element_size), prev, element_size);
            --j;
        }
        memcpy(array + (j * element_size), buffer, element_size);
    }

    free(buffer);
}

size_t ac_bin_search_double(
    const double *array,
    double value,
    size_t start,
    size_t end
) {
    if (array == NULL) {
        return start;
    }

    while (start < end) {
        size_t mid = start + ((end - start) / 2U);
        if (array[mid] < value) {
            start = mid + 1U;
        } else {
            end = mid;
        }
    }
    return start;
}

void ac_insertion_sort_opt_double(double *data, size_t size) {
    if (data == NULL || size < 2U) {
        return;
    }

    for (size_t i = 1; i < size; ++i) {
        double current = data[i];
        size_t position = ac_bin_search_double(data, current, 0U, i);

        for (size_t j = i; j > position; --j) {
            data[j] = data[j - 1U];
        }
        data[position] = current;
    }
}

void ac_insertion_sort_double(double *data, size_t size) {
    if (data == NULL || size < 2U) {
        return;
    }

    for (size_t i = 0; i < size; ++i) {
        size_t k = i;
        while (k > 0U && data[k - 1U] > data[k]) {
            double tmp = data[k - 1U];
            data[k - 1U] = data[k];
            data[k] = tmp;
            --k;
        }
    }
}
