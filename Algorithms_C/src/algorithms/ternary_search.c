#include "algorithms_c/algorithms/ternary_search.h"
#include <math.h>
#include <stddef.h>

static void ac_normalise_interval(double *start, double *end) {
    double tmp;
    if (*end < *start) {
        tmp = *start;
        *start = *end;
        *end = tmp;
    }
}

static int ac_ternary_search_internal(
    ac_unimodal_function func,
    void *user_data,
    double start,
    double end,
    double eps,
    double *out_position,
    int find_max
) {
    double left_edge;
    double right_edge;
    double middle_left;
    double middle_right;
    if (func == NULL || out_position == NULL || eps <= 0.0) {
        return -1;
    }

    left_edge = start;
    right_edge = end;
    ac_normalise_interval(&left_edge, &right_edge);
    middle_left = left_edge;
    middle_right = right_edge;

    while (fabs(right_edge - left_edge) >= eps) {
        double span;
        double left_value;
        double right_value;
        span = right_edge - left_edge;
        middle_left = left_edge + (span / 3.0);
        middle_right = left_edge + ((2.0 * span) / 3.0);
        left_value = func(middle_left, user_data);
        right_value = func(middle_right, user_data);

        if (find_max) {
            if (right_value > left_value) {
                left_edge = middle_left;
            } else {
                right_edge = middle_right;
            }
        } else {
            if (right_value < left_value) {
                left_edge = middle_left;
            } else {
                right_edge = middle_right;
            }
        }
    }

    *out_position = (middle_left + middle_right) / 2.0;
    return 0;
}

int ac_ternary_search_min(
    ac_unimodal_function func,
    void *user_data,
    double start,
    double end,
    double eps,
    double *out_position
) {
    return ac_ternary_search_internal(
        func, user_data, start, end, eps, out_position, 0
    );
}

int ac_ternary_search_max(
    ac_unimodal_function func,
    void *user_data,
    double start,
    double end,
    double eps,
    double *out_position
) {
    return ac_ternary_search_internal(
        func, user_data, start, end, eps, out_position, 1
    );
}
