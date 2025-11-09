#include "algorithms_c/structures/stack.h"
#include "algorithms_c/utils/minunit.h"

static void test_stack_push_pop(void) {
    ac_stack stack;
    MU_ASSERT(ac_stack_init(&stack, sizeof(int)) == AC_VECTOR_OK);

    for (int value = 0; value < 10; ++value) {
        MU_ASSERT(ac_stack_push(&stack, &value) == AC_VECTOR_OK);
    }

    for (int value = 9; value >= 0; --value) {
        int out = 0;
        MU_ASSERT(ac_stack_top(&stack, &out) == AC_VECTOR_OK);
        MU_ASSERT(out == value);
        MU_ASSERT(ac_stack_pop(&stack, &out) == AC_VECTOR_OK);
        MU_ASSERT(out == value);
    }

    MU_ASSERT(ac_stack_empty(&stack));
    ac_stack_destroy(&stack);
}

void register_stack_tests(void) {
    run_test(test_stack_push_pop);
}
