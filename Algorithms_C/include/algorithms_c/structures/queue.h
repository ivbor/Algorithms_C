#ifndef ALGORITHMS_C_STRUCTURES_QUEUE_H
#define ALGORITHMS_C_STRUCTURES_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *data;
    size_t element_size;
    size_t capacity;
    size_t size;
    size_t head;
    size_t tail;
} ac_queue;

int ac_queue_init(
    ac_queue *queue,
    size_t element_size,
    size_t initial_capacity
);
void ac_queue_destroy(ac_queue *queue);
bool ac_queue_empty(const ac_queue *queue);
size_t ac_queue_size(const ac_queue *queue);
size_t ac_queue_capacity(const ac_queue *queue);
int ac_queue_reserve(ac_queue *queue, size_t new_capacity);
int ac_queue_enqueue(ac_queue *queue, const void *value);
int ac_queue_dequeue(ac_queue *queue, void *out_value);
int ac_queue_peek(const ac_queue *queue, void *out_value);

#ifdef __cplusplus
}
#endif

#endif
