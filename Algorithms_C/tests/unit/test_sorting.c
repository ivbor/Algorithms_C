#include <stdlib.h>
#include "algorithms_c/algorithms/sorting.h"
#include "algorithms_c/structures/vector.h"
#include "algorithms_c/utils/minunit.h"

static void assert_sorted(const int *array, size_t size) {
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

static void test_selection_sort(void) {
    int data[48];
    fill_random(data, 48);
    ac_selection_sort(data, 48, sizeof(int), ac_compare_int);
    assert_sorted(data, 48);
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

static void test_quick_sort(void) {
    int data[128];
    fill_random(data, 128);
    ac_quick_sort(data, 128, sizeof(int), ac_compare_int);
    assert_sorted(data, 128);
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

static void test_selection_sort_invalid_arguments_are_noop(void) {
    int data[] = {3, 1, 2};
    ac_selection_sort(NULL, 3, sizeof(int), ac_compare_int);
    ac_selection_sort(data, 3, 0, ac_compare_int);
    ac_selection_sort(data, 3, sizeof(int), NULL);
    MU_ASSERT(data[0] == 3);
    MU_ASSERT(data[1] == 1);
    MU_ASSERT(data[2] == 2);
}

int main(void) {
    srand(12345u);
    run_test(test_insertion_sort);
    run_test(test_selection_sort);
    run_test(test_selection_sort_handles_duplicates_and_negatives);
    run_test(test_merge_sort);
    run_test(test_quick_sort);
    run_test(test_heap_sort);
    run_test(test_counting_sort);
    run_test(test_selection_sort_invalid_arguments_are_noop);
    return summary();
}
