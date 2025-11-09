#include "algorithms_c/structures/queue.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void *allocate_buffer(size_t count, size_t element_size) {
    if (count == 0 || element_size == 0) {
        return NULL;
    }
    if (count > SIZE_MAX / element_size) {
        errno = ENOMEM;
        return NULL;
    }
    return malloc(count * element_size);
}

static void queue_memcpy(ac_queue *queue, size_t index, const void *source) {
    unsigned char *base = (unsigned char *)queue->data;
    size_t offset = index * queue->element_size;
    memcpy(base + offset, source, queue->element_size);
}

static void queue_memmove(ac_queue *queue, void *destination, size_t index) {
    unsigned char *base = (unsigned char *)queue->data;
    size_t offset = index * queue->element_size;
    memcpy(destination, base + offset, queue->element_size);
}

static int grow_if_needed(ac_queue *queue, size_t required_capacity) {
    if (queue->capacity >= required_capacity) {
        return 0;
    }

    size_t new_capacity = queue->capacity == 0 ? 1 : queue->capacity;
    while (new_capacity < required_capacity) {
        if (new_capacity > SIZE_MAX / 2) {
            return -ENOMEM;
        }
        new_capacity *= 2;
    }

    return ac_queue_reserve(queue, new_capacity);
}

int ac_queue_init(
    ac_queue *queue,
    size_t element_size,
    size_t initial_capacity
) {
    if (queue == NULL || element_size == 0) {
        return -EINVAL;
    }

    queue->data = allocate_buffer(initial_capacity, element_size);
    if (initial_capacity != 0 && queue->data == NULL) {
        return -ENOMEM;
    }

    queue->element_size = element_size;
    queue->capacity = initial_capacity;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    return 0;
}

void ac_queue_destroy(ac_queue *queue) {
    if (queue == NULL) {
        return;
    }

    free(queue->data);
    queue->data = NULL;
    queue->element_size = 0;
    queue->capacity = 0;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
}

bool ac_queue_empty(const ac_queue *queue) {
    return queue == NULL || queue->size == 0;
}

size_t ac_queue_size(const ac_queue *queue) {
    return queue == NULL ? 0U : queue->size;
}

size_t ac_queue_capacity(const ac_queue *queue) {
    return queue == NULL ? 0U : queue->capacity;
}

int ac_queue_reserve(ac_queue *queue, size_t new_capacity) {
    if (queue == NULL) {
        return -EINVAL;
    }

    if (new_capacity <= queue->capacity) {
        return 0;
    }

    void *new_data = allocate_buffer(new_capacity, queue->element_size);
    if (new_data == NULL) {
        return -ENOMEM;
    }

    if (queue->size > 0 && queue->data != NULL) {
        unsigned char *dst = (unsigned char *)new_data;
        for (size_t i = 0; i < queue->size; ++i) {
            size_t index = (queue->head + i) % queue->capacity;
            memcpy(
                dst + (i * queue->element_size),
                (unsigned char *)queue->data + (index * queue->element_size),
                queue->element_size
            );
        }
    }

    free(queue->data);
    queue->data = new_data;
    queue->capacity = new_capacity;
    queue->head = 0;
    queue->tail = queue->size;
    return 0;
}

int ac_queue_enqueue(ac_queue *queue, const void *value) {
    if (queue == NULL || value == NULL) {
        return -EINVAL;
    }

    int status = grow_if_needed(queue, queue->size + 1);
    if (status != 0) {
        return status;
    }

    queue_memcpy(queue, queue->tail, value);
    queue->tail =
        (queue->tail + 1) % (queue->capacity == 0 ? 1 : queue->capacity);
    queue->size++;
    return 0;
}

int ac_queue_dequeue(ac_queue *queue, void *out_value) {
    if (queue == NULL || queue->size == 0) {
        return -ENOENT;
    }

    if (out_value != NULL) {
        queue_memmove(queue, out_value, queue->head);
    }

    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    if (queue->size == 0) {
        queue->head = 0;
        queue->tail = 0;
    }
    return 0;
}

int ac_queue_peek(const ac_queue *queue, void *out_value) {
    if (queue == NULL || queue->size == 0 || out_value == NULL) {
        return -ENOENT;
    }

    const unsigned char *base = (const unsigned char *)queue->data;
    size_t offset = queue->head * queue->element_size;
    memcpy(out_value, base + offset, queue->element_size);
    return 0;
}
