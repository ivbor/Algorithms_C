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

static void test_counting_sort(void) {
    int data[100];
    for (size_t i = 0; i < 100; ++i) {
        data[i] = (int)(rand() % 201) - 100;
    }
    ac_counting_sort_int(data, 100, -100, 100);
    assert_sorted(data, 100);
}

int main(void) {
    srand(12345u);
    run_test(test_insertion_sort);
    run_test(test_merge_sort);
    run_test(test_quick_sort);
    run_test(test_counting_sort);
    return summary();
}
