#include "algorithms_c/structures/stack.h"
#include <stdlib.h>

/**
 * @file stack.c
 * @brief Documentation-first stack wrapper built on top of ::ac_vector.
 *
 * Long description: the original Python ``Stack`` type is intentionally
 * straightforward, and this C translation keeps that simplicity by delegating
 * all storage details to the vector module.  Instead of repeating low-level
 * allocation code, each function documents why the vector call preserves stack
 * semantics (LIFO ordering, O(1) top access, and amortised O(1) push/pop).
 *
 * The heavy inline commentary is intentional: it mirrors the educational style
 * used in the Python repository where prose is part of the API contract.
 */

/*
 * Short description: initialize stack storage and invariants.
 * Long description: a stack is considered valid once its underlying vector is
 * initialized. We intentionally funnel initialization through ::ac_vector_init
 * so validation and growth policy remain centralized in one container module.
 * Signature: int ac_stack_init(ac_stack *stack, size_t element_size)
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

/*
 * Short description: release stack-owned memory.
 * Long description: destruction is delegated to ::ac_vector_destroy to ensure
 * the same cleanup rules are used everywhere in the project. Calling destroy on
 * NULL remains a no-op for defensive, idempotent teardown paths.
 * Signature: void ac_stack_destroy(ac_stack *stack)
 */
void ac_stack_destroy(ac_stack *stack) {
    if (stack == NULL) {
        return;
    }

    /* Destroying the underlying vector releases all stack-owned memory. */
    ac_vector_destroy(&stack->storage);
}

/*
 * Short description: check whether stack contains no elements.
 * Long description: for safety in test harnesses and cleanup flows, NULL is
 * treated as empty. This mirrors other container helpers in the C translation
 * that favor defensive behavior over crashes for invalid handles.
 * Signature: bool ac_stack_empty(const ac_stack *stack)
 */
bool ac_stack_empty(const ac_stack *stack) {
    /* Null stacks are treated as empty for defensive call sites. */
    return stack == NULL || ac_vector_empty(&stack->storage);
}

/*
 * Short description: read current number of pushed elements.
 * Long description: the size is tracked by the vector backend and retrieved in
 * constant time. NULL returns 0 so instrumentation callers can probe safely.
 * Signature: size_t ac_stack_size(const ac_stack *stack)
 */
size_t ac_stack_size(const ac_stack *stack) {
    return stack == NULL ? 0U : ac_vector_size(&stack->storage);
}

/*
 * Short description: push a value onto stack top.
 * Long description: appending to the vector tail is exactly stack push in a
 * contiguous-array representation. This preserves LIFO order while reusing the
 * vector's amortised growth strategy and allocation checks.
 * Signature: int ac_stack_push(ac_stack *stack, const void *value)
 */
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

/*
 * Short description: remove top element, optionally returning its value.
 * Long description: popping from stack top maps to removing vector tail. The
 * optional output pointer preserves ergonomic parity with other structures
 * where removal and retrieval are combined in one operation. Signature: int
 * ac_stack_pop(ac_stack *stack, void *out_value)
 */
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

/*
 * Short description: inspect current top element without mutation.
 * Long description: top/peek reads index ``size - 1`` and leaves storage
 * untouched. Explicit NULL and empty checks make the function predictable in
 * tests and align with the project's defensive API conventions.
 * Signature: int ac_stack_top(const ac_stack *stack, void *out_value)
 */
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
