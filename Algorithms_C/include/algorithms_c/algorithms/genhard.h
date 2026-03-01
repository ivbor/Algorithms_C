#ifndef ALGORITHMS_C_ALGORITHMS_GENHARD_H
#define ALGORITHMS_C_ALGORITHMS_GENHARD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file genhard.h
 * @brief Knapsack instance generation helpers inspired by the Python
 * ``genhard`` example.
 */

/**
 * @brief Generate an uncorrelated 0/1-knapsack instance.
 *
 * This helper mirrors the educational ``genhard`` idea: profits and weights are
 * sampled independently from ``[1, range_max]`` using a deterministic PRNG
 * seed. Capacity is derived as ``capacity_ratio`` of the total generated
 * weights and is clamped to ``[1, sum(weights)]`` when items are present.
 *
 * @param n Number of items.
 * @param range_max Maximum generated value for each weight/profit (inclusive).
 * @param seed Deterministic seed for reproducible generation.
 * @param capacity_ratio Ratio in ``[0.0, 1.0]`` used to derive capacity.
 * @param out_profits Output profits array of length ``n``.
 * @param out_weights Output weights array of length ``n``.
 * @param out_capacity Output pointer receiving derived capacity.
 * @return ``0`` on success, ``-1`` for invalid arguments.
 * @signature int ac_genhard_generate_uncorrelated(size_t n, int range_max,
 *                                                 unsigned int seed,
 *                                                 double capacity_ratio,
 *                                                 int *out_profits,
 *                                                 int *out_weights,
 *                                                 int *out_capacity)
 */
int ac_genhard_generate_uncorrelated(
    size_t n,
    int range_max,
    unsigned int seed,
    double capacity_ratio,
    int *out_profits,
    int *out_weights,
    int *out_capacity
);

/**
 * @brief Generate a weakly-correlated 0/1-knapsack instance.
 *
 * Profits are generated near weights (``profit ~= weight``) with bounded noise,
 * which mirrors classic weak-correlation benchmarks from knapsack literature.
 * Capacity derivation follows the same rule as the uncorrelated generator.
 *
 * @param n Number of items.
 * @param range_max Maximum generated base value for each weight (inclusive).
 * @param seed Deterministic seed for reproducible generation.
 * @param capacity_ratio Ratio in ``[0.0, 1.0]`` used to derive capacity.
 * @param out_profits Output profits array of length ``n``.
 * @param out_weights Output weights array of length ``n``.
 * @param out_capacity Output pointer receiving derived capacity.
 * @return ``0`` on success, ``-1`` for invalid arguments.
 * @signature int ac_genhard_generate_weakly_correlated(size_t n, int range_max,
 *                                                      unsigned int seed,
 *                                                      double capacity_ratio,
 *                                                      int *out_profits,
 *                                                      int *out_weights,
 *                                                      int *out_capacity)
 */
int ac_genhard_generate_weakly_correlated(
    size_t n,
    int range_max,
    unsigned int seed,
    double capacity_ratio,
    int *out_profits,
    int *out_weights,
    int *out_capacity
);

/**
 * @brief Generate a strongly-correlated 0/1-knapsack instance.
 *
 * Weights are sampled from ``[1, range_max]`` and profits are computed as
 * ``weight + correlation_offset``. This mirrors the classic strongly-correlated
 * benchmark family where value and weight are almost linearly coupled.
 *
 * @param n Number of items.
 * @param range_max Maximum generated weight value (inclusive).
 * @param correlation_offset Non-negative additive offset applied to profits.
 * @param seed Deterministic seed for reproducible generation.
 * @param capacity_ratio Ratio in ``[0.0, 1.0]`` used to derive capacity.
 * @param out_profits Output profits array of length ``n``.
 * @param out_weights Output weights array of length ``n``.
 * @param out_capacity Output pointer receiving derived capacity.
 * @return ``0`` on success, ``-1`` for invalid arguments.
 * @signature int ac_genhard_generate_strongly_correlated(
 *               size_t n,
 *               int range_max,
 *               int correlation_offset,
 *               unsigned int seed,
 *               double capacity_ratio,
 *               int *out_profits,
 *               int *out_weights,
 *               int *out_capacity)
 */
int ac_genhard_generate_strongly_correlated(
    size_t n,
    int range_max,
    int correlation_offset,
    unsigned int seed,
    double capacity_ratio,
    int *out_profits,
    int *out_weights,
    int *out_capacity
);

#ifdef __cplusplus
}
#endif

#endif
