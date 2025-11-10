#ifndef ALGORITHMS_C_STRUCTURES_DEQUE_H
#define ALGORITHMS_C_STRUCTURES_DEQUE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file deque.h
 * @brief Generic double-ended queue (deque) container.
 *
 * The Python repository exposes a richly documented ``Deque`` type built on
 * top of the ``DoubleNode`` abstraction.  For the C port we opt for a compact
 * ring-buffer representation that mirrors the behaviour of the Python class
 * while integrating smoothly with the rest of the C translation.  The
 * documentation in this header intentionally follows the verbose style used in
 * the Python project: every public function includes a short overview, a
 * detailed description of the underlying algorithmic choices, and an explicit
 * signature section so that the intent is clear even without reading the
 * implementation.  This verbosity keeps the educational nature of the project
 * front and centre and results in more comments than code by design.
 */

/**
 * @struct ac_deque
 * @brief Owning container that stores elements in a circular buffer.
 *
 * The deque keeps track of the buffer ``capacity`` separately from the number
 * of live elements ``size``.  ``head`` indexes the logical front element (the
 * one that would be removed by ::ac_deque_pop_front) while ``tail`` always
 * identifies the next free slot after the logical back element (the insertion
 * point used by ::ac_deque_push_back).  Both indices wrap modulo
 * ``capacity`` so that push and pop operations run in constant time.
 */
typedef struct {
    /** Pointer to the contiguous ring-buffer storing raw bytes. */
    void *data;
    /** Size in bytes of an individual stored element. */
    size_t element_size;
    /** Total number of element slots available in @ref data. */
    size_t capacity;
    /** Number of elements currently stored in the deque. */
    size_t size;
    /** Index of the logical front element within the ring-buffer. */
    size_t head;
    /** Index immediately past the logical back element. */
    size_t tail;
} ac_deque;

/**
 * @brief Create a deque with an optional pre-allocated capacity.
 *
 * The constructor mirrors ``Deque.__init__`` from the Python code base.  When
 * ``initial_capacity`` is zero the deque starts empty and allocates lazily as
 * soon as the first element is pushed.  Non-zero capacities reserve the
 * required storage up front which matches scenarios where the user knows the
 * approximate workload.
 *
 * @param deque Destination structure that will hold the deque state.
 * @param element_size Size in bytes of each stored element; must be non-zero.
 * @param initial_capacity Number of elements to reserve immediately.
 * @return ``0`` on success, ``-EINVAL`` if the arguments are invalid, and
 *         ``-ENOMEM`` if allocation fails.
 * @signature int ac_deque_init(ac_deque *deque, size_t element_size,
 *                              size_t initial_capacity)
 */
int ac_deque_init(ac_deque *deque, size_t element_size, size_t initial_capacity);

/**
 * @brief Release every resource owned by ``deque`` and reset it to zero state.
 *
 * Destroying a deque mirrors ``Deque.pop_*`` draining in the Python version:
 * any allocated buffer is freed, the bookkeeping fields are cleared, and the
 * pointer becomes safe for reuse after another call to ::ac_deque_init.
 *
 * @param deque Pointer to an initialised deque structure.  ``NULL`` is a
 *              no-op to keep shutdown paths simple.
 * @signature void ac_deque_destroy(ac_deque *deque)
 */
void ac_deque_destroy(ac_deque *deque);

/**
 * @brief Determine whether the deque currently stores no elements.
 *
 * @param deque Pointer to an initialised deque structure (nullable).
 * @return ``true`` when ``deque`` has zero elements, otherwise ``false``.
 * @signature bool ac_deque_empty(const ac_deque *deque)
 */
bool ac_deque_empty(const ac_deque *deque);

/**
 * @brief Report the number of stored elements.
 *
 * The function keeps the signature parity with ``__len__`` from the Python
 * deque.
 *
 * @param deque Pointer to an initialised deque structure (nullable).
 * @return Element count.  ``0`` is returned for ``NULL`` pointers to simplify
 *         error handling in callers.
 * @signature size_t ac_deque_size(const ac_deque *deque)
 */
size_t ac_deque_size(const ac_deque *deque);

/**
 * @brief Return the currently allocated capacity of the deque.
 *
 * This mirrors the explicit capacity tracking present in the other translated
 * structures (vector and queue) and exposes the growth behaviour used by the
 * ring-buffer implementation.
 *
 * @param deque Pointer to an initialised deque structure (nullable).
 * @return Total number of element slots currently available.
 * @signature size_t ac_deque_capacity(const ac_deque *deque)
 */
size_t ac_deque_capacity(const ac_deque *deque);

/**
 * @brief Explicitly grow the backing storage if ``new_capacity`` exceeds the
 *        current allocation.
 *
 * The function relinearises the logical order of elements so that the ring
 * buffer becomes a simple array starting at index zero in the new storage.
 * This behaviour matches the semantics of the Python implementation which
 * never exposes the physical layout to the caller.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param new_capacity Target number of element slots required by the caller.
 * @return ``0`` on success, ``-EINVAL`` for invalid arguments, ``-ENOMEM`` if
 *         allocation fails.
 * @signature int ac_deque_reserve(ac_deque *deque, size_t new_capacity)
 */
int ac_deque_reserve(ac_deque *deque, size_t new_capacity);

/**
 * @brief Insert ``value`` at the logical back of the deque.
 *
 * Algorithmically this mirrors ``Deque.push_back``: the tail index identifies
 * the insertion slot and automatically wraps around the ring-buffer.  Capacity
 * is doubled on demand which keeps amortised insertion cost ``O(1)``.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param value Address of the element to copy into the container.
 * @return ``0`` on success, ``-EINVAL`` for invalid arguments, ``-ENOMEM`` if
 *         the deque cannot grow.
 * @signature int ac_deque_push_back(ac_deque *deque, const void *value)
 */
int ac_deque_push_back(ac_deque *deque, const void *value);

/**
 * @brief Insert ``value`` at the logical front of the deque.
 *
 * The operation is the counterpart of ``Deque.push_front`` in the Python code.
 * It decrements (with wrap-around) the ``head`` index and copies the provided
 * value into the freed slot.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param value Address of the element to copy into the container.
 * @return ``0`` on success, ``-EINVAL`` for invalid arguments, ``-ENOMEM`` if
 *         the deque cannot grow.
 * @signature int ac_deque_push_front(ac_deque *deque, const void *value)
 */
int ac_deque_push_front(ac_deque *deque, const void *value);

/**
 * @brief Remove and optionally return the element from the front of the deque.
 *
 * Matches ``Deque.pop_front`` and returns ``-ENOENT`` when called on an empty
 * container.  When ``out_value`` is ``NULL`` the value is discarded without
 * copying which keeps error handling inexpensive in fast paths.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param out_value Optional pointer that receives a copy of the removed value.
 * @return ``0`` on success, ``-ENOENT`` if the deque is empty, ``-EINVAL`` for
 *         invalid arguments.
 * @signature int ac_deque_pop_front(ac_deque *deque, void *out_value)
 */
int ac_deque_pop_front(ac_deque *deque, void *out_value);

/**
 * @brief Remove and optionally return the element from the back of the deque.
 *
 * The method pairs with ``Deque.pop_back``.  The tail index is rewound with
 * wrap-around semantics before copying out the value.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param out_value Optional pointer that receives a copy of the removed value.
 * @return ``0`` on success, ``-ENOENT`` if the deque is empty, ``-EINVAL`` for
 *         invalid arguments.
 * @signature int ac_deque_pop_back(ac_deque *deque, void *out_value)
 */
int ac_deque_pop_back(ac_deque *deque, void *out_value);

/**
 * @brief Copy the element at the logical front without removing it.
 *
 * Equivalent to ``Queue.peek`` in the Python code base.  The operation
 * requires ``out_value`` to be non-``NULL`` so that callers always receive a
 * valid copy on success.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param out_value Destination buffer that receives the front element.
 * @return ``0`` on success, ``-ENOENT`` if the deque is empty, ``-EINVAL`` for
 *         invalid arguments.
 * @signature int ac_deque_front(const ac_deque *deque, void *out_value)
 */
int ac_deque_front(const ac_deque *deque, void *out_value);

/**
 * @brief Copy the element at the logical back without removing it.
 *
 * Mirrors ``Stack.back`` from the Python implementation but in the context of
 * the deque container.  ``out_value`` must point to a valid buffer.
 *
 * @param deque Pointer to an initialised deque structure.
 * @param out_value Destination buffer that receives the back element.
 * @return ``0`` on success, ``-ENOENT`` if the deque is empty, ``-EINVAL`` for
 *         invalid arguments.
 * @signature int ac_deque_back(const ac_deque *deque, void *out_value)
 */
int ac_deque_back(const ac_deque *deque, void *out_value);

/**
 * @brief Reset the deque to an empty state without releasing capacity.
 *
 * Equivalent to clearing both ends in the Python code.  Retaining the
 * allocated buffer keeps subsequent insertions amortised ``O(1)``.
 *
 * @param deque Pointer to an initialised deque structure.
 * @signature void ac_deque_clear(ac_deque *deque)
 */
void ac_deque_clear(ac_deque *deque);

#ifdef __cplusplus
}
#endif

#endif
