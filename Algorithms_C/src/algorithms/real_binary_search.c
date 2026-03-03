#include "algorithms_c/algorithms/real_binary_search.h"
#include <math.h>
#include <stddef.h>

int ac_real_binary_search(
    ac_real_function_fn func,
    void *context,
    double func_value,
    double left_edge,
    double right_edge,
    double eps,
    bool check_reachable,
    double *result
) {
    if (func == NULL || result == NULL || eps <= 0.0 ||
        right_edge < left_edge) {
        return -1;
    }

    if (left_edge == right_edge) {
        *result = left_edge;
        return 0;
    }

    const double left_value = func(left_edge, context);
    const double right_value = func(right_edge, context);
    const bool is_ascending = left_value < right_value;

    if (check_reachable) {
        const double min_value = is_ascending ? left_value : right_value;
        const double max_value = is_ascending ? right_value : left_value;
        if (func_value < min_value || func_value > max_value) {
            return -2;
        }
    }

    double left = left_edge;
    double right = right_edge;

    while (fabs(right - left) >= eps) {
        const double middle = (left + right) / 2.0;
        const double middle_value = func(middle, context);

        if (is_ascending) {
            if (middle_value < func_value) {
                left = middle;
            } else {
                right = middle;
            }
        } else {
            if (middle_value > func_value) {
                left = middle;
            } else {
                right = middle;
            }
        }
    }

    *result = (left + right) / 2.0;
    return 0;
}
