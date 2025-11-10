#include <errno.h>
#include "algorithms_c/structures/deque.h"
#include "algorithms_c/utils/minunit.h"

static void test_deque_push_back_and_pop_front(void) {
    ac_deque deque;
    MU_ASSERT(ac_deque_init(&deque, sizeof(int), 0) == 0);

    for (int value = 0; value < 100; ++value) {
        MU_ASSERT(ac_deque_push_back(&deque, &value) == 0);
    }

    MU_ASSERT(ac_deque_size(&deque) == 100);

    for (int expected = 0; expected < 100; ++expected) {
        int front = -1;
        MU_ASSERT(ac_deque_front(&deque, &front) == 0);
        MU_ASSERT(front == expected);
        MU_ASSERT(ac_deque_pop_front(&deque, &front) == 0);
        MU_ASSERT(front == expected);
    }

    MU_ASSERT(ac_deque_empty(&deque));
    ac_deque_destroy(&deque);
}

static void test_deque_push_front_and_pop_back(void) {
    ac_deque deque;
    MU_ASSERT(ac_deque_init(&deque, sizeof(int), 4) == 0);

    for (int value = 0; value < 10; ++value) {
        MU_ASSERT(ac_deque_push_front(&deque, &value) == 0);
    }

    MU_ASSERT(ac_deque_size(&deque) == 10);

    for (int expected = 0; expected < 10; ++expected) {
        int back = -1;
        MU_ASSERT(ac_deque_back(&deque, &back) == 0);
        MU_ASSERT(back == expected);
        MU_ASSERT(ac_deque_pop_back(&deque, &back) == 0);
        MU_ASSERT(back == expected);
    }

    MU_ASSERT(ac_deque_empty(&deque));
    ac_deque_destroy(&deque);
}

static void test_deque_wraparound_and_reserve(void) {
    ac_deque deque;
    MU_ASSERT(ac_deque_init(&deque, sizeof(int), 3) == 0);

    for (int value = 0; value < 3; ++value) {
        MU_ASSERT(ac_deque_push_back(&deque, &value) == 0);
    }

    for (int i = 0; i < 2; ++i) {
        int out = -1;
        MU_ASSERT(ac_deque_pop_front(&deque, &out) == 0);
        MU_ASSERT(out == i);
    }

    int wrap_values[] = {100, 101, 102};
    for (size_t i = 0; i < sizeof(wrap_values) / sizeof(wrap_values[0]); ++i) {
        MU_ASSERT(ac_deque_push_back(&deque, &wrap_values[i]) == 0);
    }

    MU_ASSERT(ac_deque_size(&deque) == 4);
    MU_ASSERT(ac_deque_reserve(&deque, 16) == 0);
    MU_ASSERT(ac_deque_capacity(&deque) >= 5);

    int front = -1;
    MU_ASSERT(ac_deque_front(&deque, &front) == 0);
    MU_ASSERT(front == 2);

    int expected_values[] = {2, 100, 101, 102};
    for (size_t i = 0; i < sizeof(expected_values) / sizeof(expected_values[0]); ++i) {
        int out = -1;
        MU_ASSERT(ac_deque_pop_front(&deque, &out) == 0);
        MU_ASSERT(out == expected_values[i]);
    }

    MU_ASSERT(ac_deque_empty(&deque));
    ac_deque_destroy(&deque);
}

static void test_deque_error_conditions(void) {
    MU_ASSERT(ac_deque_init(NULL, sizeof(int), 0) == -EINVAL);

    ac_deque deque;
    MU_ASSERT(ac_deque_init(&deque, 0, 0) == -EINVAL);
    MU_ASSERT(ac_deque_init(&deque, sizeof(int), 0) == 0);

    int out = 0;
    MU_ASSERT(ac_deque_front(&deque, &out) == -ENOENT);
    MU_ASSERT(ac_deque_back(&deque, &out) == -ENOENT);
    MU_ASSERT(ac_deque_pop_front(&deque, &out) == -ENOENT);
    MU_ASSERT(ac_deque_pop_back(&deque, &out) == -ENOENT);
    MU_ASSERT(ac_deque_push_back(&deque, NULL) == -EINVAL);
    MU_ASSERT(ac_deque_push_front(&deque, NULL) == -EINVAL);

    int value = 42;
    MU_ASSERT(ac_deque_push_back(&deque, &value) == 0);
    MU_ASSERT(ac_deque_front(&deque, &out) == 0);
    MU_ASSERT(out == value);
    MU_ASSERT(ac_deque_back(&deque, &out) == 0);
    MU_ASSERT(out == value);

    ac_deque_clear(&deque);
    MU_ASSERT(ac_deque_empty(&deque));

    ac_deque_destroy(&deque);
}

int main(void) {
    run_test(test_deque_push_back_and_pop_front);
    run_test(test_deque_push_front_and_pop_back);
    run_test(test_deque_wraparound_and_reserve);
    run_test(test_deque_error_conditions);
    return summary();
}
