#include "algorithms_c/structures/queue.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * The queue mirrors the Python ``Queue`` class but is optimised using a
 * circular buffer.  Inline comments document how indices wrap, how memory
 * growth is handled, and why the translation stays faithful to the higher-level
 * semantics.  The verbosity is intentional: educational notes are prioritised
 * over terseness.
 */

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

// Copy a single element into the buffer respecting the configured element size.
static void queue_memcpy(ac_queue *queue, size_t index, const void *source) {
    unsigned char *base = (unsigned char *)queue->data;
    size_t offset = index * queue->element_size;
    memcpy(base + offset, source, queue->element_size);
}

// Extract a single element from the buffer for dequeue/peek operations.
static void queue_memmove(ac_queue *queue, void *destination, size_t index) {
    unsigned char *base = (unsigned char *)queue->data;
    size_t offset = index * queue->element_size;
    memcpy(destination, base + offset, queue->element_size);
}

// Double the allocation until the queue can store ``required_capacity`` items.
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

/*
 * Short description: initialize queue storage and ring-buffer invariants.
 * Long description: initialization allocates optional initial storage and
 * establishes a canonical empty state (size=0, head=tail=0).  This mirrors the
 * Python Queue constructor semantics while exposing explicit memory ownership.
 * Signature: int ac_queue_init(ac_queue *queue, size_t element_size,
 *                              size_t initial_capacity)
 */
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

/*
 * Short description: release queue-owned resources.
 * Long description: destruction frees the ring buffer and resets all metadata
 * fields so repeated destroy/init cycles in tests remain deterministic. NULL is
 * accepted as a defensive no-op.
 * Signature: void ac_queue_destroy(ac_queue *queue)
 */
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

/*
 * Short description: check emptiness in constant time.
 * Long description: the queue tracks `size` explicitly, so emptiness does not
 * rely on head/tail arithmetic. NULL is treated as empty for defensive callers.
 * Signature: bool ac_queue_empty(const ac_queue *queue)
 */
bool ac_queue_empty(const ac_queue *queue) {
    return queue == NULL || queue->size == 0;
}

/*
 * Short description: return the number of queued elements.
 * Long description: this accessor is O(1) because size is maintained eagerly on
 * enqueue/dequeue transitions. NULL yields 0 for observability convenience.
 * Signature: size_t ac_queue_size(const ac_queue *queue)
 */
size_t ac_queue_size(const ac_queue *queue) {
    return queue == NULL ? 0U : queue->size;
}

/*
 * Short description: return currently allocated slot count.
 * Long description: capacity reflects allocated slots, not logical size. It is
 * useful for diagnostics and stress tests that assert growth behavior.
 * Signature: size_t ac_queue_capacity(const ac_queue *queue)
 */
size_t ac_queue_capacity(const ac_queue *queue) {
    return queue == NULL ? 0U : queue->capacity;
}

/*
 * Short description: grow backing storage to at least `new_capacity`.
 * Long description: when growth is required, elements are copied in logical
 * FIFO order into new contiguous storage and indices are re-based to head=0.
 * This keeps subsequent wrap-around arithmetic straightforward and
 * deterministic. Signature: int ac_queue_reserve(ac_queue *queue, size_t
 * new_capacity)
 */
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

    /*
     * The queue may currently wrap around the end of the old buffer. During
     * reallocation we linearise the logical order so that ``head`` becomes
     * zero in the new storage. This keeps subsequent push/pop logic simple and
     * mirrors how the Python implementation conceptually stores items in a
     * contiguous list.
     */
    if (queue->size > 0 && queue->data != NULL) {
        unsigned char *dst = (unsigned char *)new_data;
        size_t capacity = queue->capacity == 0 ? 1 : queue->capacity;
        for (size_t i = 0; i < queue->size; ++i) {
            size_t index = (queue->head + i) % capacity;
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

/*
 * Short description: append one element at queue tail.
 * Long description: enqueue triggers growth when needed, writes bytes at tail,
 * then advances tail modulo capacity. This preserves FIFO ordering while
 * keeping per-operation overhead O(1) amortized. Signature: int
 * ac_queue_enqueue(ac_queue *queue, const void *value)
 */
int ac_queue_enqueue(ac_queue *queue, const void *value) {
    if (queue == NULL || value == NULL) {
        return -EINVAL;
    }

    int status = grow_if_needed(queue, queue->size + 1);
    if (status != 0) {
        return status;
    }

    /* Place the element at the current tail slot, then advance tail. */
    queue_memcpy(queue, queue->tail, value);
    queue->tail =
        (queue->tail + 1) % (queue->capacity == 0 ? 1 : queue->capacity);
    queue->size++;
    return 0;
}

/*
 * Short description: remove and optionally return the front element.
 * Long description: dequeue reads from head, advances head modulo capacity, and
 * decrements size. When the queue becomes empty, indices are reset to zero to
 * keep state stable for debugging and deterministic test assertions.
 * Signature: int ac_queue_dequeue(ac_queue *queue, void *out_value)
 */
int ac_queue_dequeue(ac_queue *queue, void *out_value) {
    if (queue == NULL || queue->size == 0) {
        return -ENOENT;
    }

    /* Optionally expose the dequeued value before moving the head pointer. */
    if (out_value != NULL) {
        queue_memmove(queue, out_value, queue->head);
    }

    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    /*
     * Reset indices when the queue becomes empty. This is not strictly
     * required for correctness, but it simplifies debugging and keeps state
     * deterministic for tests and docs.
     */
    if (queue->size == 0) {
        queue->head = 0;
        queue->tail = 0;
    }
    return 0;
}

/*
 * Short description: inspect the front element without removing it.
 * Long description: peek copies bytes from the current head slot and leaves all
 * indices unchanged, providing read-only observation of FIFO front state.
 * Signature: int ac_queue_peek(const ac_queue *queue, void *out_value)
 */
int ac_queue_peek(const ac_queue *queue, void *out_value) {
    if (queue == NULL || queue->size == 0 || out_value == NULL) {
        return -ENOENT;
    }

    const unsigned char *base = (const unsigned char *)queue->data;
    size_t offset = queue->head * queue->element_size;
    memcpy(out_value, base + offset, queue->element_size);
    return 0;
}
