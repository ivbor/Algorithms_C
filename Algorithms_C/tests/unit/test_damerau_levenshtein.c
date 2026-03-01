#include "algorithms_c/algorithms/damerau_levenshtein.h"
#include "algorithms_c/utils/minunit.h"

static void test_equal_strings(void) {
    MU_ASSERT(ac_damerau_levenshtein_distance("algorithm", "algorithm") == 0);
}

static void test_basic_operations(void) {
    MU_ASSERT(ac_damerau_levenshtein_distance("kitten", "sitting") == 3);
    MU_ASSERT(ac_damerau_levenshtein_distance("book", "back") == 2);
}

static void test_transposition(void) {
    MU_ASSERT(ac_damerau_levenshtein_distance("ab", "ba") == 1);
    MU_ASSERT(ac_damerau_levenshtein_distance("ca", "ac") == 1);
}

static void test_reference_examples(void) {
    MU_ASSERT(ac_damerau_levenshtein_distance("sunday", "saturday") == 3);
    MU_ASSERT(ac_damerau_levenshtein_distance("abcdef", "abcfde") == 2);
}

static void test_compatibility_alias(void) {
    MU_ASSERT(ac_damerau_levenstein_distance("ab", "ba") == 1);
    MU_ASSERT(ac_damerau_levenstein_distance("kitten", "sitting") == 3);
    MU_ASSERT(ac_damerau_levenstein_distance(NULL, "s") == -1);
}

static void test_empty_and_invalid(void) {
    MU_ASSERT(ac_damerau_levenshtein_distance("", "") == 0);
    MU_ASSERT(ac_damerau_levenshtein_distance("abc", "") == 3);
    MU_ASSERT(ac_damerau_levenshtein_distance("", "abc") == 3);
    MU_ASSERT(ac_damerau_levenshtein_distance(NULL, "abc") == -1);
    MU_ASSERT(ac_damerau_levenshtein_distance("abc", NULL) == -1);
}

int main(void) {
    run_test(test_equal_strings);
    run_test(test_basic_operations);
    run_test(test_transposition);
    run_test(test_reference_examples);
    run_test(test_compatibility_alias);
    run_test(test_empty_and_invalid);
    return summary();
}
