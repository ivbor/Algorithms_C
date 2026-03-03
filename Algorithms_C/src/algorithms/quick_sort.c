#include <math.h>
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

static int ac_compare_double_local(const void *lhs, const void *rhs) {
    const double left = *(const double *)lhs;
    const double right = *(const double *)rhs;
    if (left < right) {
        return -1;
    }
    if (left > right) {
        return 1;
    }
    return 0;
}

void ac_quick_sort_double(double *data, size_t size) {
    ac_quick_sort(data, size, sizeof(double), ac_compare_double_local);
}

int ac_split_double_by_pivot(
    double *data,
    size_t left,
    size_t right,
    double pivot,
    size_t *out_left,
    size_t *out_right
) {
    if (data == NULL || out_left == NULL || out_right == NULL || left > right) {
        return -1;
    }

    size_t middle = left;
    for (size_t i = left; i < right; ++i) {
        if (data[i] < pivot) {
            double tmp = data[i];
            data[i] = data[middle];
            data[middle] = tmp;
            ++middle;
        }
    }

    size_t new_left = middle;
    for (size_t i = new_left; i < right; ++i) {
        if (data[i] == pivot) {
            double tmp = data[i];
            data[i] = data[middle];
            data[middle] = tmp;
            ++middle;
        }
    }

    *out_left = new_left;
    *out_right = middle;
    return 0;
}

int ac_closest_to_average_double(
    const double *data,
    size_t left,
    size_t right,
    double *out_value
) {
    if (data == NULL || out_value == NULL || left >= right) {
        return -1;
    }

    double sum = 0.0;
    for (size_t i = left; i < right; ++i) {
        sum += data[i];
    }
    double average = sum / (double)(right - left);

    size_t best = left;
    double best_dist = fabs(data[left] - average);
    for (size_t i = left + 1U; i < right; ++i) {
        double dist = fabs(data[i] - average);
        if (dist < best_dist) {
            best_dist = dist;
            best = i;
        }
    }

    *out_value = data[best];
    return 0;
}
