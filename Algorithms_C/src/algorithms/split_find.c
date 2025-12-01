#include "algorithms_c/algorithms/split_find.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static void ac_swap_double(double *lhs, double *rhs) {
    double tmp;
    tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

static void ac_split(
    double *array,
    size_t left_edge,
    size_t right_edge,
    double pivot,
    size_t *out_left,
    size_t *out_right
) {
    size_t middle;
    size_t i;
    middle = left_edge;
    for (i = left_edge; i < right_edge; ++i) {
        if (array[i] < pivot) {
            ac_swap_double(&array[i], &array[middle]);
            middle += 1U;
        }
    }

    *out_left = middle;
    for (i = middle; i < right_edge; ++i) {
        double difference;
        difference = array[i] - pivot;
        if (difference < 0.0) {
            difference = -difference;
        }
        if (difference <= 1e-14) {
            ac_swap_double(&array[i], &array[middle]);
            middle += 1U;
        }
    }
    *out_right = middle;
}

static double ac_split_find_recursive(
    double *array,
    size_t left_edge,
    size_t right_edge,
    size_t index
) {
    size_t pivot_offset;
    double pivot;
    size_t new_left_edge;
    size_t new_right_edge;

    pivot_offset = (size_t)(rand() % (int)(right_edge - left_edge));
    pivot = array[left_edge + pivot_offset];
    ac_split(
        array, left_edge, right_edge, pivot, &new_left_edge, &new_right_edge
    );

    if ((right_edge - left_edge) <= (new_right_edge - new_left_edge)) {
        return array[index];
    }

    if (index < new_left_edge) {
        return ac_split_find_recursive(array, left_edge, new_left_edge, index);
    }

    return ac_split_find_recursive(array, new_left_edge, right_edge, index);
}

int ac_split_find(double *array, size_t size, size_t index, double *out_value) {
    if (array == NULL || out_value == NULL || index >= size) {
        return -1;
    }

    *out_value = ac_split_find_recursive(array, 0U, size, index);
    return 0;
}
