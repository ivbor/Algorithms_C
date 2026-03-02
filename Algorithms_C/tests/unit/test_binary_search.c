#include "algorithms_c/algorithms/binary_search.h"
#include "algorithms_c/utils/minunit.h"

static void test_binary_search_found(void) {
    int data[] = {1, 3, 5, 7, 9};
    int target = 7;
    ptrdiff_t index =
        ac_binary_search(data, 5, sizeof(int), &target, ac_compare_int);
    MU_ASSERT(index == 3);
}

static void test_binary_search_not_found(void) {
    int data[] = {1, 3, 5, 7, 9};
    int target = 2;
    ptrdiff_t index =
        ac_binary_search(data, 5, sizeof(int), &target, ac_compare_int);
    MU_ASSERT(index == -1);
}

static void test_bounds(void) {
    int data[] = {1, 2, 2, 2, 5};
    int target = 2;
    size_t lower =
        ac_lower_bound(data, 5, sizeof(int), &target, ac_compare_int);
    size_t upper =
        ac_upper_bound(data, 5, sizeof(int), &target, ac_compare_int);
    MU_ASSERT(lower == 1);
    MU_ASSERT(upper == 4);
}

static void test_bounds_invalid_input_returns_size(void) {
    int data[] = {1, 2, 3};
    int target = 2;

    MU_ASSERT(
        ac_lower_bound(NULL, 3, sizeof(int), &target, ac_compare_int) == 3
    );
    MU_ASSERT(ac_upper_bound(data, 3, sizeof(int), NULL, ac_compare_int) == 3);
    MU_ASSERT(ac_lower_bound(data, 3, 0, &target, ac_compare_int) == 3);
    MU_ASSERT(ac_upper_bound(data, 3, sizeof(int), &target, NULL) == 3);
}

static void test_bin_search_int_recursive_mode(void) {
    int data[] = {1, 3, 5, 7, 9};
    MU_ASSERT(ac_bin_search_int(data, 5, 7, 0) == 1);
    MU_ASSERT(ac_bin_search_int(data, 5, 2, 0) == 0);
}

static void test_bin_search_int_iterative_mode(void) {
    int data[] = {1, 3, 5, 7, 9};
    MU_ASSERT(ac_bin_search_int(data, 5, 9, 1) == 1);
    MU_ASSERT(ac_bin_search_int(data, 5, -1, 1) == 0);
}

static void test_bin_search_int_invalid_input(void) {
    MU_ASSERT(ac_bin_search_int(NULL, 0, 1, 0) == 0);
}

int main(void) {
    run_test(test_binary_search_found);
    run_test(test_binary_search_not_found);
    run_test(test_bounds);
    run_test(test_bounds_invalid_input_returns_size);
    run_test(test_bin_search_int_recursive_mode);
    run_test(test_bin_search_int_iterative_mode);
    run_test(test_bin_search_int_invalid_input);
    return summary();
}
