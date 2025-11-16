#include <stdio.h>
#include "algorithms_c/structures/slist.h"
#include "algorithms_c/utils/minunit.h"

static int compare_ints(const void *lhs, const void *rhs, void *user_data) {
    (void)user_data;
    int left = lhs != NULL ? *(const int *)lhs : 0;
    int right = rhs != NULL ? *(const int *)rhs : 0;
    if (left == right) {
        return 0;
    }
    return left < right ? -1 : 1;
}

static int accumulate_const(const void *value, void *user_data) {
    int *sum = user_data;
    *sum += *(const int *)value;
    return 0;
}

static int increment_mutable(void *value, void *user_data) {
    int delta = *(int *)user_data;
    int *target = value;
    *target += delta;
    return 0;
}

static void test_slist_append_and_iteration(void) {
    ac_slist list;
    MU_ASSERT(ac_slist_init(&list, sizeof(int)) == AC_SLIST_OK);

    for (int i = 0; i < 10; ++i) {
        MU_ASSERT(ac_slist_append(&list, &i) == AC_SLIST_OK);
    }

    MU_ASSERT(ac_slist_size(&list) == 10);
    MU_ASSERT(!ac_slist_empty(&list));

    for (int i = 0; i < 10; ++i) {
        int value = 0;
        MU_ASSERT(ac_slist_get(&list, (size_t)i, &value) == AC_SLIST_OK);
        MU_ASSERT(value == i);
    }

    int sum = 0;
    MU_ASSERT(ac_slist_for_each_const(&list, accumulate_const, &sum) == AC_SLIST_OK);
    MU_ASSERT(sum == 45);

    ac_slist_destroy(&list);
}

static void test_slist_insert_prepend_and_erase(void) {
    ac_slist list;
    MU_ASSERT(ac_slist_init(&list, sizeof(int)) == AC_SLIST_OK);

    for (int i = 0; i < 5; ++i) {
        MU_ASSERT(ac_slist_append(&list, &i) == AC_SLIST_OK);
    }

    int front = 42;
    MU_ASSERT(ac_slist_prepend(&list, &front) == AC_SLIST_OK);

    int middle = 99;
    MU_ASSERT(ac_slist_insert(&list, 3, &middle) == AC_SLIST_OK);

    int expected[] = {42, 0, 1, 99, 2, 3, 4};
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
        int value = 0;
        MU_ASSERT(ac_slist_get(&list, i, &value) == AC_SLIST_OK);
        MU_ASSERT(value == expected[i]);
    }

    int removed = 0;
    MU_ASSERT(ac_slist_erase(&list, 0, &removed) == AC_SLIST_OK);
    MU_ASSERT(removed == 42);
    MU_ASSERT(ac_slist_pop_back(&list, &removed) == AC_SLIST_OK);
    MU_ASSERT(removed == 4);

    MU_ASSERT(ac_slist_size(&list) == 5);

    ac_slist_destroy(&list);
}

static void test_slist_front_back_pop(void) {
    ac_slist list;
    MU_ASSERT(ac_slist_init(&list, sizeof(int)) == AC_SLIST_OK);

    int values[] = {7, 8, 9};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        MU_ASSERT(ac_slist_append(&list, &values[i]) == AC_SLIST_OK);
    }

    int peek = 0;
    MU_ASSERT(ac_slist_front(&list, &peek) == AC_SLIST_OK);
    MU_ASSERT(peek == 7);
    MU_ASSERT(ac_slist_back(&list, &peek) == AC_SLIST_OK);
    MU_ASSERT(peek == 9);

    MU_ASSERT(ac_slist_pop_front(&list, &peek) == AC_SLIST_OK);
    MU_ASSERT(peek == 7);
    MU_ASSERT(ac_slist_pop_back(&list, &peek) == AC_SLIST_OK);
    MU_ASSERT(peek == 9);

    MU_ASSERT(ac_slist_size(&list) == 1);
    MU_ASSERT(ac_slist_pop_back(&list, &peek) == AC_SLIST_OK);
    MU_ASSERT(peek == 8);
    MU_ASSERT(ac_slist_empty(&list));

    ac_slist_destroy(&list);
}

static void test_slist_contains_and_iteration_mutation(void) {
    ac_slist list;
    MU_ASSERT(ac_slist_init(&list, sizeof(int)) == AC_SLIST_OK);

    for (int value = 0; value < 6; ++value) {
        MU_ASSERT(ac_slist_append(&list, &value) == AC_SLIST_OK);
    }

    int needle = 3;
    MU_ASSERT(ac_slist_contains(&list, &needle, compare_ints, NULL));
    needle = 10;
    MU_ASSERT(!ac_slist_contains(&list, &needle, compare_ints, NULL));

    int delta = 5;
    MU_ASSERT(ac_slist_for_each(&list, increment_mutable, &delta) == AC_SLIST_OK);

    for (size_t i = 0; i < ac_slist_size(&list); ++i) {
        int value = 0;
        MU_ASSERT(ac_slist_get(&list, i, &value) == AC_SLIST_OK);
        MU_ASSERT(value == (int)i + 5);
    }

    ac_slist_destroy(&list);
}

static void test_slist_error_conditions(void) {
    ac_slist list;
    MU_ASSERT(ac_slist_init(NULL, sizeof(int)) == AC_SLIST_ERR_ARGUMENT);
    MU_ASSERT(ac_slist_init(&list, 0) == AC_SLIST_ERR_ARGUMENT);
    MU_ASSERT(ac_slist_init(&list, sizeof(int)) == AC_SLIST_OK);

    int value = 1;
    MU_ASSERT(ac_slist_append(NULL, &value) == AC_SLIST_ERR_ARGUMENT);
    MU_ASSERT(ac_slist_append(&list, NULL) == AC_SLIST_ERR_ARGUMENT);

    MU_ASSERT(ac_slist_get(&list, 0, &value) == AC_SLIST_ERR_INDEX);
    MU_ASSERT(ac_slist_pop_front(&list, &value) == AC_SLIST_ERR_EMPTY);
    MU_ASSERT(ac_slist_back(&list, &value) == AC_SLIST_ERR_EMPTY);
    MU_ASSERT(ac_slist_erase(&list, 0, &value) == AC_SLIST_ERR_EMPTY);

    MU_ASSERT(ac_slist_prepend(&list, &value) == AC_SLIST_OK);
    MU_ASSERT(ac_slist_insert(&list, 2, &value) == AC_SLIST_ERR_INDEX);

    ac_slist_destroy(&list);
}

int main(void) {
    run_test(test_slist_append_and_iteration);
    run_test(test_slist_insert_prepend_and_erase);
    run_test(test_slist_front_back_pop);
    run_test(test_slist_contains_and_iteration_mutation);
    run_test(test_slist_error_conditions);
    return summary();
}
