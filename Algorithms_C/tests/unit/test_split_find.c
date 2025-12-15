#include <stdlib.h>
#include "algorithms_c/algorithms/split_find.h"
#include "algorithms_c/utils/minunit.h"

static void test_split_find_sorted_input(void) {
    double values[] = {1.0, 2.0, 3.0, 4.0};
    double result;
    int rc;

    rc = ac_split_find(values, 4U, 2U, &result);

    MU_ASSERT(rc == 0);
    MU_ASSERT(result == 3.0);
}

static void test_split_find_unsorted_input(void) {
    double values[] = {4.0, 1.0, 3.0, 2.0};
    double expected_sorted[] = {1.0, 2.0, 3.0, 4.0};
    double result;
    int rc;

    rc = ac_split_find(values, 4U, 1U, &result);

    MU_ASSERT(rc == 0);
    MU_ASSERT(result == expected_sorted[1]);
}

static void test_split_find_handles_duplicates(void) {
    double values[] = {5.0, 5.0, 5.0, 5.0};
    double result;
    int rc;

    rc = ac_split_find(values, 4U, 3U, &result);

    MU_ASSERT(rc == 0);
    MU_ASSERT(result == 5.0);
}

static void test_split_find_rejects_bad_input(void) {
    double value = 0.0;
    int rc;

    rc = ac_split_find(NULL, 0U, 0U, &value);
    MU_ASSERT(rc == -1);

    rc = ac_split_find(&value, 1U, 2U, &value);
    MU_ASSERT(rc == -1);

    rc = ac_split_find(&value, 1U, 0U, NULL);
    MU_ASSERT(rc == -1);
}

int main(void) {
    srand(42);

    run_test(test_split_find_sorted_input);
    run_test(test_split_find_unsorted_input);
    run_test(test_split_find_handles_duplicates);
    run_test(test_split_find_rejects_bad_input);
    return summary();
}
