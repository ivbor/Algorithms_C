#ifndef ALGORITHMS_C_ALGORITHMS_MINKNAP_H
#define ALGORITHMS_C_ALGORITHMS_MINKNAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file minknap.h
 * @brief 0/1 knapsack helper translated from the Python DP examples.
 *
 * The Python repository includes a pedagogical ``minknap`` example in the
 * dynamic-programming section.  This C header exposes a reusable counterpart
 * that keeps the same teaching intent: inputs are explicit arrays of profits
 * and weights, and the optional ``selected`` vector reveals which items were
 * picked in an optimal solution.
 */

/**
 * @brief Compute the maximum achievable profit under a weight constraint.
 *
 * The routine solves the classic 0/1 knapsack problem for ``n`` items where
 * each item may be selected at most once.  A dynamic-programming table over
 * capacities is used and, when ``selected`` is non-``NULL``, the function also
 * reconstructs an optimal selection mask.
 *
 * @param n Number of items.
 * @param profits Profit/value array of length ``n``.
 * @param weights Weight array of length ``n``.
 * @param capacity Maximum allowed total weight (must be non-negative).
 * @param selected Optional output array of length ``n`` filled with
 * ``0``/``1``.
 * @return Maximum profit on success, or ``-1`` for invalid input/allocation
 *         failure.
 * @signature int ac_minknap_max_profit(size_t n, const int *profits,
 *                                      const int *weights, int capacity,
 *                                      int *selected)
 */
int ac_minknap_max_profit(
    size_t n,
    const int *profits,
    const int *weights,
    int capacity,
    int *selected
);

/**
 * @brief Compatibility wrapper mirroring the historical ``minknap`` signature.
 *
 * This wrapper preserves the classic parameter ordering used in the translated
 * dynamic-programming examples: ``(n, p, w, x, c)`` where ``x`` is an optional
 * output selection array.
 *
 * @param n Number of items.
 * @param p Profit array.
 * @param w Weight array.
 * @param x Optional selected-item output array.
 * @param c Capacity.
 * @return Maximum profit on success, or ``-1`` on invalid input/allocation
 *         failure.
 * @signature int ac_minknap(int n, const int *p, const int *w, int *x, int c)
 */
int ac_minknap(int n, const int *p, const int *w, int *x, int c);

#ifdef __cplusplus
}
#endif

#endif
