#include "algorithms_c/algorithms/bounds.h"
#include "algorithms_c/utils/minunit.h"

static void test_bounds_basic_duplicates(void) {
    const int data[] = {-3, -1, -1, 0, 2, 2, 2, 9};
    MU_ASSERT(ac_bounds_lower_int(data, 8, -1) == 1);
    MU_ASSERT(ac_bounds_upper_int(data, 8, -1) == 2);
    MU_ASSERT(ac_bounds_lower_int(data, 8, 2) == 4);
    MU_ASSERT(ac_bounds_upper_int(data, 8, 2) == 6);
}

static void test_bounds_absent_values(void) {
    const int data[] = {1, 3, 5, 7};
    MU_ASSERT(ac_bounds_lower_int(data, 4, 2) == -1);
    MU_ASSERT(ac_bounds_upper_int(data, 4, 2) == -1);
    MU_ASSERT(ac_bounds_lower_int(data, 4, 9) == -1);
    MU_ASSERT(ac_bounds_upper_int(data, 4, -5) == -1);
}

static void test_bounds_invalid_inputs(void) {
    const int data[] = {4};
    MU_ASSERT(ac_bounds_lower_int(NULL, 1, 4) == -1);
    MU_ASSERT(ac_bounds_upper_int(NULL, 1, 4) == -1);
    MU_ASSERT(ac_bounds_lower_int(data, 0, 4) == -1);
    MU_ASSERT(ac_bounds_upper_int(data, 0, 4) == -1);
}

int main(void) {
    run_test(test_bounds_basic_duplicates);
    run_test(test_bounds_absent_values);
    run_test(test_bounds_invalid_inputs);
    return summary();
}
