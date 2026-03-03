#ifndef ALGORITHMS_C_ALGORITHMS_REAL_BINARY_SEARCH_H
#define ALGORITHMS_C_ALGORITHMS_REAL_BINARY_SEARCH_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file real_binary_search.h
 * @brief Binary search over real numbers for monotonic functions.
 *
 * This header translates ``Algorithms_Python/real_bin_search.py`` into a C
 * callback-driven API. The helper locates an ``x`` such that ``func(x)`` is
 * approximately equal to a target value within the requested epsilon.
 */

/** Callback signature for real-valued monotonic functions. */
typedef double (*ac_real_function_fn)(double x, void *context);

/**
 * @brief Search for ``x`` in ``[left_edge, right_edge]`` so ``func(x)`` is near
 *        ``func_value``.
 *
 * The search assumes monotonic behaviour over the interval. When
 * ``check_reachable`` is true, the routine validates that the target value lies
 * between endpoint evaluations and fails otherwise.
 *
 * @param func Monotonic function to evaluate.
 * @param context Opaque pointer forwarded to ``func``.
 * @param func_value Target function value.
 * @param left_edge Left boundary of the search interval.
 * @param right_edge Right boundary of the search interval.
 * @param eps Desired precision; must be positive.
 * @param check_reachable Enable endpoint reachability check.
 * @param result Output location for the computed x-value.
 * @return ``0`` on success, ``-1`` on invalid arguments, ``-2`` when
 *         ``check_reachable`` is enabled and the target is outside interval
 *         bounds.
 * @signature int ac_real_binary_search(ac_real_function_fn func, void *context,
 *                                      double func_value, double left_edge,
 *                                      double right_edge, double eps,
 *                                      bool check_reachable, double *result)
 */
int ac_real_binary_search(
    ac_real_function_fn func,
    void *context,
    double func_value,
    double left_edge,
    double right_edge,
    double eps,
    bool check_reachable,
    double *result
);

#ifdef __cplusplus
}
#endif

#endif
