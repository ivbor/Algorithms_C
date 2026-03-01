#include "algorithms_c/structures/stack.h"
#include <stdlib.h>

/**
 * The Python Stack class is intentionally simple and educational. This C
 * translation preserves that spirit by implementing stack semantics as explicit
 * wrappers over the already-documented vector container. The wrappers are thin,
 * but the comments explain why each delegation is correct and what guarantees
 * callers can rely on.
 */

int ac_stack_init(ac_stack *stack, size_t element_size) {
    if (stack == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    /*
     * Initialization is delegated to vector construction so element-size
     * validation and allocation policy stay centralized in one module.
     */
    return ac_vector_init(&stack->storage, element_size);
}

void ac_stack_destroy(ac_stack *stack) {
    if (stack == NULL) {
        return;
    }

    /* Destroying the underlying vector releases all stack-owned memory. */
    ac_vector_destroy(&stack->storage);
}

bool ac_stack_empty(const ac_stack *stack) {
    /* Null stacks are treated as empty for defensive call sites. */
    return stack == NULL || ac_vector_empty(&stack->storage);
}

size_t ac_stack_size(const ac_stack *stack) {
    return stack == NULL ? 0U : ac_vector_size(&stack->storage);
}

int ac_stack_push(ac_stack *stack, const void *value) {
    if (stack == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    /*
     * Pushing onto a stack is equivalent to appending to vector tail. This
     * keeps push/pop operations ``O(1)`` amortized.
     */
    return ac_vector_push_back(&stack->storage, value);
}

int ac_stack_pop(ac_stack *stack, void *out_value) {
    if (stack == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    /*
     * Popping from a stack is equivalent to removing vector tail. When
     * out_value is non-null the removed bytes are copied to caller storage.
     */
    return ac_vector_pop_back(&stack->storage, out_value);
}

int ac_stack_top(const ac_stack *stack, void *out_value) {
    if (stack == NULL || out_value == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (ac_stack_empty(stack)) {
        return AC_VECTOR_ERR_EMPTY;
    }

    /*
     * Top inspection reads the element at index size-1 without mutating the
     * vector, matching standard stack peek semantics.
     */
    return ac_vector_get(
        &stack->storage, ac_vector_size(&stack->storage) - 1, out_value
    );
}
