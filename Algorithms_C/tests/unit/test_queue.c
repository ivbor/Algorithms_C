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

int main(void) {
    run_test(test_queue_enqueue_dequeue);
    return summary();
}
