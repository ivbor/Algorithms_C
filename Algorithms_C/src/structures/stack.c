#include "algorithms_c/structures/stack.h"
#include <stdlib.h>

/**
 * Rather than re-implement a bespoke linked structure, the stack piggybacks on
 * the documented vector helper.  This file exists mostly as thin wrappers, yet
 * we keep the commentary explicit so that the correspondence with the Python
 * ``Stack`` class remains evident.
 */

int ac_stack_init(ac_stack *stack, size_t element_size) {
    if (stack == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    return ac_vector_init(&stack->storage, element_size);
}

void ac_stack_destroy(ac_stack *stack) {
    if (stack == NULL) {
        return;
    }

    ac_vector_destroy(&stack->storage);
}

bool ac_stack_empty(const ac_stack *stack) {
    return stack == NULL || ac_vector_empty(&stack->storage);
}

size_t ac_stack_size(const ac_stack *stack) {
    return stack == NULL ? 0U : ac_vector_size(&stack->storage);
}

int ac_stack_push(ac_stack *stack, const void *value) {
    if (stack == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }
    return ac_vector_push_back(&stack->storage, value);
}

int ac_stack_pop(ac_stack *stack, void *out_value) {
    if (stack == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }
    return ac_vector_pop_back(&stack->storage, out_value);
}

int ac_stack_top(const ac_stack *stack, void *out_value) {
    if (stack == NULL || out_value == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (ac_stack_empty(stack)) {
        return AC_VECTOR_ERR_EMPTY;
    }

    return ac_vector_get(
        &stack->storage, ac_vector_size(&stack->storage) - 1, out_value
    );
}
