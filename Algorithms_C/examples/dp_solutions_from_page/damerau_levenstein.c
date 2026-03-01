#include "algorithms_c/algorithms/damerau_levenshtein.h"

/*
 * Legacy compatibility wrapper for the dynamic-programming example translated
 * from the Python repository's DP page solutions. The public library now
 * exposes the reusable helper `ac_damerau_levenshtein_distance`; this wrapper
 * preserves the original educational function name used by the example file.
 */
int DamerauLevensteinDistance(char *s1, char *s2) {
    return ac_damerau_levenshtein_distance(s1, s2);
}
