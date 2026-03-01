#include "algorithms_c/algorithms/damerau_levenshtein.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static size_t min3(size_t a, size_t b, size_t c) {
    size_t m = a < b ? a : b;
    return m < c ? m : c;
}

int ac_damerau_levenshtein_distance(const char *lhs, const char *rhs) {
    if (lhs == NULL || rhs == NULL) {
        return -1;
    }

    size_t lhs_len = strlen(lhs);
    size_t rhs_len = strlen(rhs);

    if (lhs_len > SIZE_MAX - 1 || rhs_len > SIZE_MAX - 1) {
        return -1;
    }

    size_t rows = lhs_len + 1;
    size_t cols = rhs_len + 1;

    if (rows > SIZE_MAX / cols || rows * cols > SIZE_MAX / sizeof(size_t)) {
        return -1;
    }

    size_t cell_count = rows * cols;
    size_t *dp = (size_t *)malloc(cell_count * sizeof(size_t));
    if (dp == NULL) {
        return -1;
    }

#define DP(i, j) dp[((i) * cols) + (j)]

    for (size_t i = 0; i < rows; ++i) {
        DP(i, 0) = i;
    }
    for (size_t j = 0; j < cols; ++j) {
        DP(0, j) = j;
    }

    for (size_t i = 1; i < rows; ++i) {
        for (size_t j = 1; j < cols; ++j) {
            size_t cost = lhs[i - 1] == rhs[j - 1] ? 0U : 1U;
            DP(i, j) = min3(
                DP(i - 1, j) + 1U, DP(i, j - 1) + 1U, DP(i - 1, j - 1) + cost
            );

            if (i > 1 && j > 1 && lhs[i - 1] == rhs[j - 2] &&
                lhs[i - 2] == rhs[j - 1]) {
                size_t transposition = DP(i - 2, j - 2) + 1U;
                if (transposition < DP(i, j)) {
                    DP(i, j) = transposition;
                }
            }
        }
    }

    size_t distance = DP(lhs_len, rhs_len);
    free(dp);

    if (distance > (size_t)INT32_MAX) {
        return -1;
    }
    return (int)distance;
}

int ac_damerau_levenstein_distance(const char *lhs, const char *rhs) {
    return ac_damerau_levenshtein_distance(lhs, rhs);
}
