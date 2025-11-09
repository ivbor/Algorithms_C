#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

static void merge(
    unsigned char *array,
    unsigned char *buffer,
    size_t left,
    size_t mid,
    size_t right,
    size_t element_size,
    ac_compare_fn compare
) {
    size_t i = left;
    size_t j = mid;
    size_t k = left;

    while (i < mid && j < right) {
        const unsigned char *left_elem = array + (i * element_size);
        const unsigned char *right_elem = array + (j * element_size);
        if (compare(left_elem, right_elem) <= 0) {
            memcpy(buffer + (k * element_size), left_elem, element_size);
            ++i;
        } else {
            memcpy(buffer + (k * element_size), right_elem, element_size);
            ++j;
        }
        ++k;
    }

    while (i < mid) {
        memcpy(
            buffer + (k * element_size), array + (i * element_size),
            element_size
        );
        ++i;
        ++k;
    }

    while (j < right) {
        memcpy(
            buffer + (k * element_size), array + (j * element_size),
            element_size
        );
        ++j;
        ++k;
    }

    for (size_t index = left; index < right; ++index) {
        memcpy(
            array + (index * element_size), buffer + (index * element_size),
            element_size
        );
    }
}

static void merge_sort_recursive(
    unsigned char *array,
    unsigned char *buffer,
    size_t left,
    size_t right,
    size_t element_size,
    ac_compare_fn compare
) {
    if (right - left <= 1) {
        return;
    }

    size_t mid = left + ((right - left) / 2);
    merge_sort_recursive(array, buffer, left, mid, element_size, compare);
    merge_sort_recursive(array, buffer, mid, right, element_size, compare);
    merge(array, buffer, left, mid, right, element_size, compare);
}

void ac_merge_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    if (data == NULL || compare == NULL || element_size == 0) {
        return;
    }

    unsigned char *array = (unsigned char *)data;
    unsigned char *buffer = (unsigned char *)malloc(size * element_size);
    if (buffer == NULL) {
        return;
    }

    merge_sort_recursive(array, buffer, 0, size, element_size, compare);
    free(buffer);
}
