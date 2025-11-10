#include <errno.h>
#include "algorithms_c/structures/queue.h"
#include "algorithms_c/utils/minunit.h"

static void test_queue_enqueue_dequeue(void) {
    ac_queue queue;
    MU_ASSERT(ac_queue_init(&queue, sizeof(int), 0) == 0);

    for (int value = 0; value < 50; ++value) {
        MU_ASSERT(ac_queue_enqueue(&queue, &value) == 0);
    }

    MU_ASSERT(ac_queue_size(&queue) == 50);

    for (int value = 0; value < 50; ++value) {
        int out = 0;
        MU_ASSERT(ac_queue_peek(&queue, &out) == 0);
        MU_ASSERT(out == value);
        MU_ASSERT(ac_queue_dequeue(&queue, &out) == 0);
        MU_ASSERT(out == value);
    }

    MU_ASSERT(ac_queue_empty(&queue));
    ac_queue_destroy(&queue);
}

static void test_queue_reserve_and_wraparound(void) {
    ac_queue queue;
    MU_ASSERT(ac_queue_init(&queue, sizeof(int), 3) == 0);

    for (int value = 0; value < 3; ++value) {
        MU_ASSERT(ac_queue_enqueue(&queue, &value) == 0);
    }

    for (int value = 0; value < 2; ++value) {
        int out = 0;
        MU_ASSERT(ac_queue_dequeue(&queue, &out) == 0);
        MU_ASSERT(out == value);
    }

    int wrap_values[] = {3, 4};
    for (size_t i = 0; i < sizeof(wrap_values) / sizeof(wrap_values[0]); ++i) {
        MU_ASSERT(ac_queue_enqueue(&queue, &wrap_values[i]) == 0);
    }

    MU_ASSERT(ac_queue_size(&queue) == 3);

    MU_ASSERT(ac_queue_reserve(&queue, 8) == 0);
    MU_ASSERT(ac_queue_capacity(&queue) >= 5);

    int additional = 5;
    for (; additional < 8; ++additional) {
        MU_ASSERT(ac_queue_enqueue(&queue, &additional) == 0);
    }

    for (int expected = 2; expected < additional; ++expected) {
        int out = 0;
        MU_ASSERT(ac_queue_dequeue(&queue, &out) == 0);
        MU_ASSERT(out == expected);
    }

    MU_ASSERT(ac_queue_empty(&queue));
    ac_queue_destroy(&queue);
}

static void test_queue_error_conditions(void) {
    MU_ASSERT(ac_queue_init(NULL, sizeof(int), 0) == -EINVAL);

    ac_queue queue;
    MU_ASSERT(ac_queue_init(&queue, 0, 0) == -EINVAL);

    MU_ASSERT(ac_queue_init(&queue, sizeof(int), 0) == 0);

    int out = 0;
    MU_ASSERT(ac_queue_peek(&queue, &out) == -ENOENT);
    MU_ASSERT(ac_queue_dequeue(&queue, &out) == -ENOENT);
    MU_ASSERT(ac_queue_enqueue(&queue, NULL) == -EINVAL);

    int value = 42;
    MU_ASSERT(ac_queue_enqueue(&queue, &value) == 0);
    MU_ASSERT(ac_queue_peek(&queue, &out) == 0);
    MU_ASSERT(out == value);

    ac_queue_destroy(&queue);
}

int main(void) {
    run_test(test_queue_enqueue_dequeue);
    run_test(test_queue_reserve_and_wraparound);
    run_test(test_queue_error_conditions);
    return summary();
}
