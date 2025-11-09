#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

static void swap_elements(
    unsigned char *a,
    unsigned char *b,
    size_t element_size,
    unsigned char *scratch
) {
    if (a == b) {
        return;
    }
    memcpy(scratch, a, element_size);
    memcpy(a, b, element_size);
    memcpy(b, scratch, element_size);
}

static size_t partition(
    unsigned char *array,
    size_t low,
    size_t high,
    size_t element_size,
    ac_compare_fn compare,
    unsigned char *scratch
) {
    unsigned char *pivot = array + (high * element_size);
    size_t i = low;

    for (size_t j = low; j < high; ++j) {
        unsigned char *current = array + (j * element_size);
        if (compare(current, pivot) < 0) {
            swap_elements(
                array + (i * element_size), current, element_size, scratch
            );
            ++i;
        }
    }

    swap_elements(array + (i * element_size), pivot, element_size, scratch);
    return i;
}

static void quick_sort_recursive(
    unsigned char *array,
    size_t low,
    size_t high,
    size_t element_size,
    ac_compare_fn compare,
    unsigned char *scratch
) {
    if (high <= low) {
        return;
    }

    size_t pivot_index =
        partition(array, low, high, element_size, compare, scratch);
    if (pivot_index > 0) {
        quick_sort_recursive(
            array, low, pivot_index - 1, element_size, compare, scratch
        );
    }
    quick_sort_recursive(
        array, pivot_index + 1, high, element_size, compare, scratch
    );
}

void ac_quick_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    if (data == NULL || compare == NULL || element_size == 0) {
        return;
    }

    if (size <= 1) {
        return;
    }

    unsigned char *scratch = (unsigned char *)malloc(element_size);
    if (scratch == NULL) {
        return;
    }

    quick_sort_recursive(
        (unsigned char *)data, 0, size - 1, element_size, compare, scratch
    );
    free(scratch);
}
