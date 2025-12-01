#ifndef ALGORITHMS_C_STRUCTURES_QUEUE_H
#define ALGORITHMS_C_STRUCTURES_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file queue.h
 * @brief Circular-buffer based FIFO queue with extensive inline documentation.
 *
 * The Python repository describes its queue implementation using richly
 * annotated docstrings.  This header follows suit: each function explains the
 * behaviour, the algorithmic considerations, and repeats the signature so that
 * students reading only the header still gain the full context.
 */

/**
 * @struct ac_queue
 * @brief Generic first-in/first-out container backed by a ring buffer.
 *
 * ``head`` indexes the logical front element (next to dequeue), ``tail`` marks
 * the next insertion slot, and both wrap around ``capacity``.  ``size`` tracks
 * how many live elements are stored at any time.
 */
typedef struct {
    /** Pointer to the storage buffer. */
    void *data;
    /** Size in bytes of each stored element. */
    size_t element_size;
    /** Allocated number of element slots. */
    size_t capacity;
    /** Number of elements currently stored. */
    size_t size;
    /** Index of the logical front element. */
    size_t head;
    /** Index of the next insertion position. */
    size_t tail;
} ac_queue;

/**
 * @brief Initialise the queue with an optional initial capacity.
 *
 * Mirroring the Python ``Queue`` constructor, the queue can start empty and
 * grow on demand or pre-allocate storage when ``initial_capacity`` is supplied.
 *
 * @param queue Destination queue structure to initialise.
 * @param element_size Size in bytes of each element; must be non-zero.
 * @param initial_capacity Number of elements to reserve immediately.
 * @return ``0`` on success, ``-EINVAL`` for invalid arguments, ``-ENOMEM`` on
 *         allocation failure.
 * @signature int ac_queue_init(ac_queue *queue, size_t element_size,
 *                              size_t initial_capacity)
 */
int ac_queue_init(
    ac_queue *queue,
    size_t element_size,
    size_t initial_capacity
);

/**
 * @brief Release the resources owned by ``queue``.
 *
 * Frees the backing buffer and resets the structure, matching the clean-up
 * semantics of the Python implementation.
 *
 * @param queue Pointer to an initialised queue (nullable).
 * @signature void ac_queue_destroy(ac_queue *queue)
 */
void ac_queue_destroy(ac_queue *queue);

/**
 * @brief Check whether the queue contains zero elements.
 *
 * @param queue Pointer to an initialised queue (nullable).
 * @return ``true`` when empty, otherwise ``false``.
 * @signature bool ac_queue_empty(const ac_queue *queue)
 */
bool ac_queue_empty(const ac_queue *queue);

/**
 * @brief Return the number of stored elements.
 *
 * Equivalent to the ``__len__`` method in the Python class.
 *
 * @param queue Pointer to an initialised queue (nullable).
 * @return Count of elements currently enqueued.
 * @signature size_t ac_queue_size(const ac_queue *queue)
 */
size_t ac_queue_size(const ac_queue *queue);

/**
 * @brief Report the allocated capacity of the queue.
 *
 * @param queue Pointer to an initialised queue (nullable).
 * @return Number of element slots currently available without reallocation.
 * @signature size_t ac_queue_capacity(const ac_queue *queue)
 */
size_t ac_queue_capacity(const ac_queue *queue);

/**
 * @brief Explicitly grow the backing storage when ``new_capacity`` exceeds the
 *        current allocation.
 *
 * The function reorders elements so that the logical sequence becomes
 * contiguous in the new buffer.
 *
 * @param queue Pointer to an initialised queue.
 * @param new_capacity Target capacity requested by the caller.
 * @return ``0`` on success, ``-EINVAL`` for invalid arguments, ``-ENOMEM`` on
 *         allocation failure.
 * @signature int ac_queue_reserve(ac_queue *queue, size_t new_capacity)
 */
int ac_queue_reserve(ac_queue *queue, size_t new_capacity);

/**
 * @brief Add ``value`` to the back of the queue.
 *
 * Mirrors ``Queue.push`` from the Python project.  Elements are copied into the
 * ring buffer and the allocation grows on demand.
 *
 * @param queue Pointer to an initialised queue.
 * @param value Address of the element to enqueue; must not be ``NULL``.
 * @return ``0`` on success, ``-EINVAL`` for invalid arguments, ``-ENOMEM`` if
 *         the queue cannot grow.
 * @signature int ac_queue_enqueue(ac_queue *queue, const void *value)
 */
int ac_queue_enqueue(ac_queue *queue, const void *value);

/**
 * @brief Remove the element at the front of the queue.
 *
 * Behaviour mirrors ``Queue.pop``: the value is copied into ``out_value`` when
 * provided.  Dequeuing from an empty queue returns ``-ENOENT``.
 *
 * @param queue Pointer to an initialised queue.
 * @param out_value Optional destination buffer for the removed element.
 * @return ``0`` on success or ``-ENOENT`` when the queue pointer is ``NULL`` or
 *         when the queue is empty.
 * @signature int ac_queue_dequeue(ac_queue *queue, void *out_value)
 */
int ac_queue_dequeue(ac_queue *queue, void *out_value);

/**
 * @brief Copy the element at the front of the queue without removing it.
 *
 * @param queue Pointer to an initialised queue.
 * @param out_value Destination buffer that receives the peeked element.
 * @return ``0`` on success or ``-ENOENT`` when the queue is empty, the queue
 *         pointer is ``NULL``, or ``out_value`` is ``NULL``.
 * @signature int ac_queue_peek(const ac_queue *queue, void *out_value)
 */
int ac_queue_peek(const ac_queue *queue, void *out_value);

#ifdef __cplusplus
}
#endif

#endif
