#include "algorithms_c/algorithms/damerau_levenshtein.h"
#include "algorithms_c/utils/minunit.h"

/*
 * Compatibility symbol provided by the translated DP example file.
 * Kept as extern because the educational example intentionally uses its
 * historical PascalCase name.
 */
extern int DamerauLevensteinDistance(char *s1, char *s2);

static void test_wrapper_matches_library_results(void) {
    char first[] = "kitten";
    char second[] = "sitting";
    MU_ASSERT(
        DamerauLevensteinDistance(first, second) ==
        ac_damerau_levenshtein_distance(first, second)
    );
}

static void test_wrapper_transposition_case(void) {
    char first[] = "ca";
    char second[] = "ac";
    MU_ASSERT(DamerauLevensteinDistance(first, second) == 1);
}

int main(void) {
    run_test(test_wrapper_matches_library_results);
    run_test(test_wrapper_transposition_case);
    return summary();
}
