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

void register_binary_search_tests(void) {
    run_test(test_binary_search_found);
    run_test(test_binary_search_not_found);
    run_test(test_bounds);
}
