#include "algorithms_c/algorithms/genhard.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Short description: generate reproducible uncorrelated knapsack instances.
 * Long description: uses a compact LCG so tests and examples can create the
 * same pseudo-random profits/weights from a seed without external dependencies.
 * Signature: int ac_genhard_generate_uncorrelated(size_t n, int range_max,
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
) {
    if (range_max <= 0 || out_capacity == NULL ||
        (n > 0 && (out_profits == NULL || out_weights == NULL))) {
        return -1;
    }
    if (capacity_ratio < 0.0 || capacity_ratio > 1.0) {
        return -1;
    }

    uint32_t state = seed;
    long long total_weight = 0;

    for (size_t i = 0; i < n; ++i) {
        state = state * 1664525u + 1013904223u;
        int weight = (int)(state % (uint32_t)range_max) + 1;

        state = state * 1664525u + 1013904223u;
        int profit = (int)(state % (uint32_t)range_max) + 1;

        out_weights[i] = weight;
        out_profits[i] = profit;
        total_weight += weight;

        if (total_weight > INT_MAX) {
            return -1;
        }
    }

    if (n == 0) {
        *out_capacity = 0;
        return 0;
    }

    int derived = (int)(capacity_ratio * (double)total_weight);
    if (derived < 1) {
        derived = 1;
    }
    if (derived > (int)total_weight) {
        derived = (int)total_weight;
    }

    *out_capacity = derived;
    return 0;
}
