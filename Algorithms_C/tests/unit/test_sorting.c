#include <stdlib.h>
#include "algorithms_c/algorithms/sorting.h"
#include "algorithms_c/structures/vector.h"
#include "algorithms_c/utils/minunit.h"

static void assert_sorted(const int *array, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        MU_ASSERT(array[i - 1] <= array[i]);
    }
}

static void assert_sorted_double(const double *array, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        MU_ASSERT(array[i - 1] <= array[i]);
    }
}

static void fill_random(int *array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        array[i] = rand() % 1000;
    }
}

static void test_insertion_sort(void) {
    int data[32];
    fill_random(data, 32);
    ac_insertion_sort(data, 32, sizeof(int), ac_compare_int);
    assert_sorted(data, 32);
}

static void test_insertion_sort_double(void) {
    double data[] = {3.2, -1.1, 4.5, 0.0, -1.1, 2.8};
    ac_insertion_sort_double(data, sizeof(data) / sizeof(data[0]));
    assert_sorted_double(data, sizeof(data) / sizeof(data[0]));
}

static void test_insertion_sort_double_invalid_arguments_are_noop(void) {
    double data[] = {9.0, 1.0};
    ac_insertion_sort_double(NULL, 2);
    ac_insertion_sort_double(data, 0);
    MU_ASSERT(data[0] == 9.0);
    MU_ASSERT(data[1] == 1.0);
}

static void test_insertion_sort_opt_double(void) {
    double data[] = {3.2, -1.1, 4.5, 0.0, -1.1, 2.8};
    ac_insertion_sort_opt_double(data, sizeof(data) / sizeof(data[0]));
    assert_sorted_double(data, sizeof(data) / sizeof(data[0]));
}

static void test_insertion_sort_opt_double_invalid_arguments_are_noop(void) {
    double data[] = {9.0, 1.0};
    ac_insertion_sort_opt_double(NULL, 2);
    ac_insertion_sort_opt_double(data, 0);
    MU_ASSERT(data[0] == 9.0);
    MU_ASSERT(data[1] == 1.0);
}

static void test_bin_search_double(void) {
    double data[] = {-2.0, -1.0, 0.5, 3.0, 7.0};
    MU_ASSERT(ac_bin_search_double(data, -1.5, 0, 5) == 1);
    MU_ASSERT(ac_bin_search_double(data, 3.0, 0, 5) == 3);
    MU_ASSERT(ac_bin_search_double(data, 8.0, 0, 5) == 5);
}

static void test_bubble_sort(void) {
    int data[40];
    fill_random(data, 40);
    ac_bubble_sort(data, 40, sizeof(int), ac_compare_int);
    assert_sorted(data, 40);
}

static void test_selection_sort(void) {
    int data[48];
    fill_random(data, 48);
    ac_selection_sort(data, 48, sizeof(int), ac_compare_int);
    assert_sorted(data, 48);
}

static void test_shell_sort(void) {
    int data[96];
    fill_random(data, 96);
    ac_shell_sort(data, 96, sizeof(int), ac_compare_int);
    assert_sorted(data, 96);
}

static void test_comb_sort(void) {
    int data[96];
    fill_random(data, 96);
    ac_comb_sort(data, 96, sizeof(int), ac_compare_int);
    assert_sorted(data, 96);
}

static void test_selection_sort_handles_duplicates_and_negatives(void) {
    int data[] = {5, -1, 3, -1, 0, 2, 5, -7, 2};
    ac_selection_sort(
        data, sizeof(data) / sizeof(data[0]), sizeof(int), ac_compare_int
    );
    assert_sorted(data, sizeof(data) / sizeof(data[0]));
}

static void test_merge_sort(void) {
    int data[128];
    fill_random(data, 128);
    ac_merge_sort(data, 128, sizeof(int), ac_compare_int);
    assert_sorted(data, 128);
}

static void test_merge_sort_double(void) {
    double data[] = {5.5, -3.3, 1.2, 1.2, 0.0};
    ac_merge_sort_double(data, sizeof(data) / sizeof(data[0]));
    assert_sorted_double(data, sizeof(data) / sizeof(data[0]));
}

static void test_merge_sort_double_invalid_arguments_are_noop(void) {
    double data[] = {2.0, -1.0};
    ac_merge_sort_double(NULL, 2);
    ac_merge_sort_double(data, 0);
    MU_ASSERT(data[0] == 2.0);
    MU_ASSERT(data[1] == -1.0);
}

static void test_quick_sort(void) {
    int data[128];
    fill_random(data, 128);
    ac_quick_sort(data, 128, sizeof(int), ac_compare_int);
    assert_sorted(data, 128);
}

static void test_quick_sort_double(void) {
    double data[] = {4.4, -2.1, 4.4, 0.0, 7.8, -10.0};
    ac_quick_sort_double(data, sizeof(data) / sizeof(data[0]));
    assert_sorted_double(data, sizeof(data) / sizeof(data[0]));
}

static void test_quick_sort_double_invalid_arguments_are_noop(void) {
    double data[] = {5.0, 1.0};
    ac_quick_sort_double(NULL, 2);
    ac_quick_sort_double(data, 0);
    MU_ASSERT(data[0] == 5.0);
    MU_ASSERT(data[1] == 1.0);
}

static void test_heap_sort(void) {
    int data[64];
    fill_random(data, 64);
    ac_heap_sort(data, 64, sizeof(int), ac_compare_int);
    assert_sorted(data, 64);
}

static void test_counting_sort(void) {
    int data[100];
    for (size_t i = 0; i < 100; ++i) {
        data[i] = (int)(rand() % 201) - 100;
    }
    ac_counting_sort_int(data, 100, -100, 100);
    assert_sorted(data, 100);
}

static void test_count_sort_auto(void) {
    int data[] = {7, -3, 4, 4, 0, -3, 9};
    ac_count_sort_int_auto(data, sizeof(data) / sizeof(data[0]));
    assert_sorted(data, sizeof(data) / sizeof(data[0]));
}

static void test_count_sort_auto_invalid_arguments_are_noop(void) {
    int data[] = {2, 1};
    ac_count_sort_int_auto(NULL, 2);
    ac_count_sort_int_auto(data, 0);
    MU_ASSERT(data[0] == 2);
    MU_ASSERT(data[1] == 1);
}

static void test_digit_sort(void) {
    int data[] = {170, 45, 75, 90, 802, 24, 2, 66, -5, -300, 45};
    ac_digit_sort_int(data, sizeof(data) / sizeof(data[0]), 10);
    assert_sorted(data, sizeof(data) / sizeof(data[0]));
}

static void test_digit_sort_opt(void) {
    int data[] = {170, 45, 75, 90, 802, 24, 2, 66, -5, -300, 45};
    ac_digit_sort_opt_int(data, sizeof(data) / sizeof(data[0]), 10);
    assert_sorted(data, sizeof(data) / sizeof(data[0]));
}

static void test_digit_sort_opt_invalid_arguments_are_noop(void) {
    int data[] = {8, 6, 7};
    ac_digit_sort_opt_int(NULL, 3, 10);
    ac_digit_sort_opt_int(data, 3, 1);
    MU_ASSERT(data[0] == 8);
    MU_ASSERT(data[1] == 6);
    MU_ASSERT(data[2] == 7);
}

static void test_digit_sort_invalid_arguments_are_noop(void) {
    int data[] = {3, 1, 2};
    ac_digit_sort_int(NULL, 3, 10);
    ac_digit_sort_int(data, 3, 1);
    MU_ASSERT(data[0] == 3);
    MU_ASSERT(data[1] == 1);
    MU_ASSERT(data[2] == 2);
}

static void test_selection_sort_invalid_arguments_are_noop(void) {
    int data[] = {3, 1, 2};
    ac_selection_sort(NULL, 3, sizeof(int), ac_compare_int);
    ac_selection_sort(data, 3, 0, ac_compare_int);
    ac_selection_sort(data, 3, sizeof(int), NULL);
    MU_ASSERT(data[0] == 3);
    MU_ASSERT(data[1] == 1);
    MU_ASSERT(data[2] == 2);
}

static void test_bubble_sort_handles_sorted_input(void) {
    int data[] = {-3, -1, 0, 4, 9, 11};
    ac_bubble_sort(
        data, sizeof(data) / sizeof(data[0]), sizeof(int), ac_compare_int
    );
    assert_sorted(data, sizeof(data) / sizeof(data[0]));
}

static void test_bubble_sort_invalid_arguments_are_noop(void) {
    int data[] = {9, 2, 7};
    ac_bubble_sort(NULL, 3, sizeof(int), ac_compare_int);
    ac_bubble_sort(data, 3, 0, ac_compare_int);
    ac_bubble_sort(data, 3, sizeof(int), NULL);
    MU_ASSERT(data[0] == 9);
    MU_ASSERT(data[1] == 2);
    MU_ASSERT(data[2] == 7);
}

static void test_shell_sort_invalid_arguments_are_noop(void) {
    int data[] = {10, 8, 9};
    ac_shell_sort(NULL, 3, sizeof(int), ac_compare_int);
    ac_shell_sort(data, 3, 0, ac_compare_int);
    ac_shell_sort(data, 3, sizeof(int), NULL);
    MU_ASSERT(data[0] == 10);
    MU_ASSERT(data[1] == 8);
    MU_ASSERT(data[2] == 9);
}

static void test_comb_sort_invalid_arguments_are_noop(void) {
    int data[] = {12, 11, 10};
    ac_comb_sort(NULL, 3, sizeof(int), ac_compare_int);
    ac_comb_sort(data, 3, 0, ac_compare_int);
    ac_comb_sort(data, 3, sizeof(int), NULL);
    MU_ASSERT(data[0] == 12);
    MU_ASSERT(data[1] == 11);
    MU_ASSERT(data[2] == 10);
}

int main(void) {
    srand(12345u);
    run_test(test_insertion_sort);
    run_test(test_insertion_sort_double);
    run_test(test_insertion_sort_opt_double);
    run_test(test_bin_search_double);
    run_test(test_bubble_sort);
    run_test(test_selection_sort);
    run_test(test_shell_sort);
    run_test(test_comb_sort);
    run_test(test_selection_sort_handles_duplicates_and_negatives);
    run_test(test_merge_sort);
    run_test(test_merge_sort_double);
    run_test(test_quick_sort);
    run_test(test_quick_sort_double);
    run_test(test_heap_sort);
    run_test(test_counting_sort);
    run_test(test_count_sort_auto);
    run_test(test_digit_sort);
    run_test(test_digit_sort_opt);
    run_test(test_selection_sort_invalid_arguments_are_noop);
    run_test(test_bubble_sort_handles_sorted_input);
    run_test(test_bubble_sort_invalid_arguments_are_noop);
    run_test(test_shell_sort_invalid_arguments_are_noop);
    run_test(test_comb_sort_invalid_arguments_are_noop);
    run_test(test_digit_sort_invalid_arguments_are_noop);
    run_test(test_count_sort_auto_invalid_arguments_are_noop);
    run_test(test_digit_sort_opt_invalid_arguments_are_noop);
    run_test(test_insertion_sort_opt_double_invalid_arguments_are_noop);
    run_test(test_insertion_sort_double_invalid_arguments_are_noop);
    run_test(test_quick_sort_double_invalid_arguments_are_noop);
    run_test(test_merge_sort_double_invalid_arguments_are_noop);
    return summary();
}
