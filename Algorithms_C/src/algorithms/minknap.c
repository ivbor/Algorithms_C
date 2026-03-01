#include "algorithms_c/algorithms/minknap.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Short description: solve 0/1 knapsack with DP and optional reconstruction.
 * Long description: this implementation favors readability and predictable
 * invariants over micro-optimizations, mirroring the educational Python source.
 * It stores best profits for each capacity and records take/skip decisions in a
 * compact matrix so an optimal item mask can be reconstructed
 * deterministically. Signature: int ac_minknap_max_profit(size_t n, const int
 * *profits, const int *weights, int capacity, int *selected)
 */
int ac_minknap_max_profit(
    size_t n,
    const int *profits,
    const int *weights,
    int capacity,
    int *selected
) {
    if (capacity < 0) {
        return -1;
    }

    if (n > 0 && (profits == NULL || weights == NULL)) {
        return -1;
    }

    for (size_t i = 0; i < n; ++i) {
        if (weights[i] < 0 || profits[i] < 0) {
            return -1;
        }
    }

    const size_t cap = (size_t)capacity;
    if (cap > SIZE_MAX - 1) {
        return -1;
    }

    const size_t width = cap + 1;
    if (n != 0 && width > SIZE_MAX / n) {
        return -1;
    }

    int *dp = calloc(width, sizeof(int));
    int *prev = calloc(width, sizeof(int));
    unsigned char *take = NULL;
    if (n > 0) {
        take = calloc(n * width, sizeof(unsigned char));
    }
    if (dp == NULL || prev == NULL || (n > 0 && take == NULL)) {
        free(dp);
        free(prev);
        free(take);
        return -1;
    }

    for (size_t i = 0; i < n; ++i) {
        memcpy(prev, dp, width * sizeof(int));
        for (size_t c = 0; c <= cap; ++c) {
            int best = prev[c];
            int choose = 0;
            const size_t wi = (size_t)weights[i];
            if (wi <= c) {
                int candidate = prev[c - wi] + profits[i];
                if (candidate > best) {
                    best = candidate;
                    choose = 1;
                }
            }
            dp[c] = best;
            take[i * width + c] = (unsigned char)choose;
        }
    }

    if (selected != NULL) {
        memset(selected, 0, n * sizeof(int));
        size_t remaining = cap;
        for (size_t i = n; i > 0; --i) {
            size_t item = i - 1;
            if (take[item * width + remaining] != 0U) {
                selected[item] = 1;
                remaining -= (size_t)weights[item];
            }
        }
    }

    int answer = dp[cap];
    free(dp);
    free(prev);
    free(take);
    return answer;
}

/*
 * Short description: compatibility wrapper preserving classic minknap call
 * form. Long description: forwards to ac_minknap_max_profit after validating
 * that the legacy signed item count can be represented as size_t. Signature:
 * int ac_minknap(int n, const int *p, const int *w, int *x, int c)
 */
int ac_minknap(int n, const int *p, const int *w, int *x, int c) {
    if (n < 0) {
        return -1;
    }

    return ac_minknap_max_profit((size_t)n, p, w, c, x);
}
