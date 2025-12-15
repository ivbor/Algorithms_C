#ifndef ALGORITHMS_C_ALGORITHMS_TERNARY_SEARCH_H
#define ALGORITHMS_C_ALGORITHMS_TERNARY_SEARCH_H

#include <stddef.h>

typedef double (*ac_unimodal_function)(double x, void *user_data);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ternary_search.h
 * @brief Ternary search helpers for locating extrema of unimodal functions.
 *
 * The Python reference exposes two educational helpers, ``tern_search_min``
 * and ``tern_search_max``, that locate the minimum or maximum of a function
 * within a closed interval.  This header mirrors that API for the C
 * translation.  Each helper repeats its signature explicitly via ``@signature``
 * tags and accompanies the implementation with a long-form description so that
 * the translation retains the documentation-first flavour of the source
 * material.
 */

/** Default precision used by ternary search helpers when callers do not need
 * custom tolerances. */
#define AC_TERNARY_SEARCH_DEFAULT_EPS 1e-6

/**
 * @brief Locate the minimum of a unimodal function on a closed interval.
 *
 * The helper mirrors the Python ``tern_search_min`` routine: two trisection
 * points are evaluated on each iteration, and the search interval is narrowed
 * toward the side with the lower function value.  The routine stops once the
 * interval width is below ``eps`` and reports the midpoint of the final
 * bracketing segment.
 *
 * @param func Callback that evaluates the function being minimised.  The
 *             callback must not be ``NULL``.
 * @param user_data Opaque context pointer forwarded to ``func`` on every
 *                  invocation; mirrors the closure-friendly nature of the
 *                  Python original.
 * @param start Lower bound of the initial search interval.
 * @param end Upper bound of the initial search interval.  When ``end`` is less
 *            than ``start`` the bounds are swapped to keep the algorithm
 *            well-defined.
 * @param eps Positive tolerance that controls when the search terminates.
 *            ``AC_TERNARY_SEARCH_DEFAULT_EPS`` is a sensible default for most
 *            inputs.
 * @param out_position Output parameter that receives the approximate x-value
 *                     where the minimum occurs.  Must not be ``NULL``.
 * @return ``0`` on success or ``-1`` when arguments are invalid.
 * @signature int ac_ternary_search_min(ac_unimodal_function func,
 *                                      void *user_data,
 *                                      double start,
 *                                      double end,
 *                                      double eps,
 *                                      double *out_position)
 */
int ac_ternary_search_min(
    ac_unimodal_function func,
    void *user_data,
    double start,
    double end,
    double eps,
    double *out_position
);

/**
 * @brief Locate the maximum of a unimodal function on a closed interval.
 *
 * This is the mirror image of ::ac_ternary_search_min.  The algorithm still
 * samples two trisection points per iteration but keeps the half-interval with
 * the *larger* function value, converging on the peak instead of the valley.
 *
 * @param func Callback that evaluates the function being maximised.
 * @param user_data Context pointer forwarded to ``func``.
 * @param start Lower bound of the search interval.
 * @param end Upper bound of the search interval.
 * @param eps Positive tolerance that controls when iteration stops.
 * @param out_position Output parameter that receives the x-coordinate of the
 *                     maximum.
 * @return ``0`` on success or ``-1`` for invalid arguments.
 * @signature int ac_ternary_search_max(ac_unimodal_function func,
 *                                      void *user_data,
 *                                      double start,
 *                                      double end,
 *                                      double eps,
 *                                      double *out_position)
 */
int ac_ternary_search_max(
    ac_unimodal_function func,
    void *user_data,
    double start,
    double end,
    double eps,
    double *out_position
);

#ifdef __cplusplus
}
#endif

#endif
