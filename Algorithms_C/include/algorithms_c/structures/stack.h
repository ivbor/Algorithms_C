#ifndef ALGORITHMS_C_STRUCTURES_STACK_H
#define ALGORITHMS_C_STRUCTURES_STACK_H

#include <stdbool.h>
#include <stddef.h>
#include "algorithms_c/structures/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file stack.h
 * @brief LIFO stack abstraction layered atop the documented vector container.
 *
 * The stack mirrors the Python ``Stack`` class which internally reused the
 * doubly linked list primitives.  In C we leverage the already-tested vector to
 * provide amortised ``O(1)`` push/pop while keeping the API surface familiar.
 * The comments intentionally provide both concise and in-depth descriptions so
 * that learners can trace the design decisions easily.
 */

/**
 * @struct ac_stack
 * @brief Wrapper around ::ac_vector that exposes stack semantics.
 */
typedef struct {
    /** Underlying storage used to hold stack elements. */
    ac_vector storage;
} ac_stack;

/**
 * @brief Initialise a stack capable of storing elements of ``element_size``.
 *
 * Delegates to ::ac_vector_init and therefore shares the same failure modes.
 *
 * @param stack Destination stack structure to initialise.
 * @param element_size Size in bytes of each element to be stored.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION on failure.
 * @signature int ac_stack_init(ac_stack *stack, size_t element_size)
 */
int ac_stack_init(ac_stack *stack, size_t element_size);

/**
 * @brief Destroy the stack and free any owned resources.
 *
 * @param stack Pointer to an initialised stack (nullable).
 * @signature void ac_stack_destroy(ac_stack *stack)
 */
void ac_stack_destroy(ac_stack *stack);

/**
 * @brief Determine whether the stack currently stores zero elements.
 *
 * @param stack Pointer to an initialised stack (nullable).
 * @return ``true`` when empty, otherwise ``false``.
 * @signature bool ac_stack_empty(const ac_stack *stack)
 */
bool ac_stack_empty(const ac_stack *stack);

/**
 * @brief Return the number of elements currently pushed onto the stack.
 *
 * @param stack Pointer to an initialised stack (nullable).
 * @return Size of the stack.
 * @signature size_t ac_stack_size(const ac_stack *stack)
 */
size_t ac_stack_size(const ac_stack *stack);

/**
 * @brief Push ``value`` onto the top of the stack.
 *
 * Internally forwards to ::ac_vector_push_back.
 *
 * @param stack Pointer to an initialised stack.
 * @param value Address of the element to store; must not be ``NULL``.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION otherwise.
 * @signature int ac_stack_push(ac_stack *stack, const void *value)
 */
int ac_stack_push(ac_stack *stack, const void *value);

/**
 * @brief Pop the top element and optionally copy it into ``out_value``.
 *
 * @param stack Pointer to an initialised stack.
 * @param out_value Optional destination buffer that receives the removed value.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_EMPTY if the stack is
 *         empty.
 * @signature int ac_stack_pop(ac_stack *stack, void *out_value)
 */
int ac_stack_pop(ac_stack *stack, void *out_value);

/**
 * @brief Copy the element currently at the top of the stack without removal.
 *
 * @param stack Pointer to an initialised stack.
 * @param out_value Destination buffer that receives the copied value.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_EMPTY when the stack is
 *         empty.
 * @signature int ac_stack_top(const ac_stack *stack, void *out_value)
 */
int ac_stack_top(const ac_stack *stack, void *out_value);

#ifdef __cplusplus
}
#endif

#endif
