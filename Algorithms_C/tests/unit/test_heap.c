#include <errno.h>
#include <stdlib.h>
#include "algorithms_c/algorithms/compare.h"
#include "algorithms_c/structures/heap.h"
#include "algorithms_c/utils/minunit.h"

static void test_heap_push_pop(void) {
    ac_heap heap;
    MU_ASSERT(ac_heap_init(&heap, sizeof(int), ac_compare_int) == 0);

    int values[] = {5, 1, 9, -3, 7, 2};
    size_t count = sizeof(values) / sizeof(values[0]);

    size_t i = 0U;
    for (i = 0U; i < count; ++i) {
        MU_ASSERT(ac_heap_push(&heap, &values[i]) == 0);
    }

    int previous = -1000;
    for (i = 0U; i < count; ++i) {
        int current = 0;
        MU_ASSERT(ac_heap_pop(&heap, &current) == 0);
        MU_ASSERT(previous <= current);
        previous = current;
    }

    MU_ASSERT(ac_heap_empty(&heap));
    ac_heap_destroy(&heap);
}

static void test_heap_peek_and_height(void) {
    ac_heap heap;
    MU_ASSERT(
        ac_heap_with_capacity(&heap, sizeof(int), 4, ac_compare_int) == 0
    );

    int values[] = {10, 4, 15};
    size_t count = sizeof(values) / sizeof(values[0]);

    size_t i = 0U;
    for (i = 0U; i < count; ++i) {
        MU_ASSERT(ac_heap_push(&heap, &values[i]) == 0);
    }

    int peeked = 0;
    MU_ASSERT(ac_heap_peek(&heap, &peeked) == 0);
    MU_ASSERT(peeked == 4);
    MU_ASSERT(ac_heap_height(&heap) >= 2U);

    ac_heap_clear(&heap);
    MU_ASSERT(ac_heap_empty(&heap));

    ac_heap_destroy(&heap);
}

static void test_heap_from_array_and_errors(void) {
    int values[] = {8, 6, 7, 5, 3, 0, 9};
    ac_heap heap;

    MU_ASSERT(
        ac_heap_from_array(
            &heap, sizeof(int), values, sizeof(values) / sizeof(values[0]),
            ac_compare_int
        ) == 0
    );

    int previous = -1000;
    size_t i = 0U;
    for (i = 0U; i < sizeof(values) / sizeof(values[0]); ++i) {
        int current = 0;
        MU_ASSERT(ac_heap_pop(&heap, &current) == 0);
        MU_ASSERT(previous <= current);
        previous = current;
    }

    MU_ASSERT(ac_heap_pop(&heap, NULL) == -ENOENT);
    MU_ASSERT(ac_heap_peek(&heap, NULL) == -ENOENT);

    ac_heap_destroy(&heap);

    MU_ASSERT(ac_heap_init(NULL, sizeof(int), ac_compare_int) == -EINVAL);
    MU_ASSERT(ac_heap_init(&heap, 0, ac_compare_int) == -EINVAL);
    MU_ASSERT(ac_heap_init(&heap, sizeof(int), NULL) == -EINVAL);
}

int main(void) {
    run_test(test_heap_push_pop);
    run_test(test_heap_peek_and_height);
    run_test(test_heap_from_array_and_errors);
    return summary();
}
