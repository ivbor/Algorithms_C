#ifndef ALGORITHMS_C_STRUCTURES_STACK_H
#define ALGORITHMS_C_STRUCTURES_STACK_H

#include <stdbool.h>
#include <stddef.h>
#include "algorithms_c/structures/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ac_vector storage;
} ac_stack;

int ac_stack_init(ac_stack *stack, size_t element_size);
void ac_stack_destroy(ac_stack *stack);
bool ac_stack_empty(const ac_stack *stack);
size_t ac_stack_size(const ac_stack *stack);
int ac_stack_push(ac_stack *stack, const void *value);
int ac_stack_pop(ac_stack *stack, void *out_value);
int ac_stack_top(const ac_stack *stack, void *out_value);

#ifdef __cplusplus
}
#endif

#endif
