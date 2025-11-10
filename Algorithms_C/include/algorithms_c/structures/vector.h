#ifndef ALGORITHMS_C_STRUCTURES_VECTOR_H
#define ALGORITHMS_C_STRUCTURES_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file vector.h
 * @brief Educational dynamic array implementation with verbose documentation.
 *
 * The original Python project accompanies every data structure with a thorough
 * description of its invariants, failure modes, and usage.  This header mirrors
 * that approach for the generic ``ac_vector`` translation: each public API
 * surface is described twice (a short summary plus a detailed explanation) and
 * the function signature is reiterated explicitly.  The resulting prose is
 * intentionally longer than the actual C code so that the reasoning behind the
 * implementation remains transparent.
 */

/**
 * @enum ac_vector_result
 * @brief Return codes emitted by vector operations.
 *
 * These constants make error handling explicit in caller code.  They mirror the
 * ValueError and IndexError exceptions used by the Python implementation.
 */
typedef enum {
    /** Operation completed successfully. */
    AC_VECTOR_OK = 0,
    /** Memory allocation failed or invalid arguments were provided. */
    AC_VECTOR_ERR_ALLOCATION = 1,
    /** The requested index falls outside the bounds of the vector. */
    AC_VECTOR_ERR_INDEX = 2,
    /** Pop operations attempted to remove from an empty container. */
    AC_VECTOR_ERR_EMPTY = 3
} ac_vector_result;

/**
 * @struct ac_vector
 * @brief Generic dynamically-resized array storing ``element_size`` bytes.
 *
 * ``ac_vector`` maintains three main invariants: ``size`` elements are
 * initialised, ``capacity`` slots are available in the allocation pointed to by
 * ``data``, and the granularity of each element is fixed at construction time
 * via ``element_size``.  These invariants allow the implementation to reuse
 * memmove/memcpy efficiently while still supporting generic payloads.
 */
typedef struct {
    /** Pointer to the contiguous storage buffer. */
    void *data;
    /** Number of live elements currently stored. */
    size_t size;
    /** Allocated slots available without growing the buffer. */
    size_t capacity;
    /** Size in bytes of each element. */
    size_t element_size;
} ac_vector;

/**
 * @brief Initialise a vector with lazy allocation.
 *
 * The function mirrors ``Vector.__init__`` in the Python code: the vector
 * starts empty, does not allocate until an element is pushed, and records the
 * element size for subsequent operations.
 *
 * @param vec Destination vector to initialise.
 * @param element_size Size in bytes of each stored element; must be non-zero.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION when arguments
 *         are invalid.
 * @signature int ac_vector_init(ac_vector *vec, size_t element_size)
 */
int ac_vector_init(ac_vector *vec, size_t element_size);

/**
 * @brief Initialise a vector and reserve ``capacity`` slots immediately.
 *
 * Useful when the caller knows the expected workload upfront.  Behaviour
 * matches ``vector.reserve`` in the C++ standard library and ``Vector`` helper
 * utilities in the Python repository.
 *
 * @param vec Destination vector to initialise.
 * @param element_size Size in bytes of each element.
 * @param capacity Number of elements to reserve in advance.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION when
 *         allocation fails or arguments are invalid.
 * @signature int ac_vector_with_capacity(ac_vector *vec, size_t element_size,
 *                                        size_t capacity)
 */
int ac_vector_with_capacity(
    ac_vector *vec,
    size_t element_size,
    size_t capacity
);

/**
 * @brief Construct a vector by copying ``count`` elements from ``source``.
 *
 * This mirrors the Python helper that creates vectors from existing sequences.
 * The function allocates exactly ``count`` slots and performs a memcpy when the
 * source pointer is non-``NULL``.
 *
 * @param vec Destination vector to initialise.
 * @param element_size Size in bytes of each element.
 * @param source Pointer to the data to copy (nullable when ``count`` is zero).
 * @param count Number of elements to copy.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION on failure.
 * @signature int ac_vector_from_array(ac_vector *vec, size_t element_size,
 *                                     const void *source, size_t count)
 */
int ac_vector_from_array(
    ac_vector *vec,
    size_t element_size,
    const void *source,
    size_t count
);

/**
 * @brief Release the resources owned by ``vec``.
 *
 * Frees the allocation (if any) and resets all bookkeeping fields to zero so
 * that the structure can be reused with another call to ::ac_vector_init.
 *
 * @param vec Pointer to an initialised vector (nullable).
 * @signature void ac_vector_destroy(ac_vector *vec)
 */
void ac_vector_destroy(ac_vector *vec);

/**
 * @brief Return the number of stored elements.
 *
 * Equivalent to ``len(vector)`` in the Python version.  ``NULL`` pointers yield
 * zero to keep guard code simple.
 *
 * @param vec Pointer to a vector.
 * @return Current size of the vector.
 * @signature size_t ac_vector_size(const ac_vector *vec)
 */
size_t ac_vector_size(const ac_vector *vec);

/**
 * @brief Return the number of elements that fit without further allocation.
 *
 * Exposes the growth behaviour so that callers can reason about amortised cost.
 *
 * @param vec Pointer to a vector (nullable).
 * @return Current capacity in elements.
 * @signature size_t ac_vector_capacity(const ac_vector *vec)
 */
size_t ac_vector_capacity(const ac_vector *vec);

/**
 * @brief Check whether the vector currently stores zero elements.
 *
 * @param vec Pointer to a vector (nullable).
 * @return ``true`` when empty, otherwise ``false``.
 * @signature bool ac_vector_empty(const ac_vector *vec)
 */
bool ac_vector_empty(const ac_vector *vec);

/**
 * @brief Expose a mutable pointer to the underlying buffer.
 *
 * Provided for interoperability with low-level code that needs contiguous
 * storage.  No ownership transfer occurs.
 *
 * @param vec Pointer to a vector.
 * @return Mutable pointer to the first element or ``NULL`` for invalid inputs.
 * @signature void *ac_vector_data(ac_vector *vec)
 */
void *ac_vector_data(ac_vector *vec);

/**
 * @brief Expose a read-only pointer to the underlying buffer.
 *
 * Mirrors ::ac_vector_data but for const contexts.
 *
 * @param vec Pointer to a vector.
 * @return Const pointer to the first element or ``NULL`` for invalid inputs.
 * @signature const void *ac_vector_const_data(const ac_vector *vec)
 */
const void *ac_vector_const_data(const ac_vector *vec);

/**
 * @brief Ensure the vector can accommodate at least ``new_capacity`` elements.
 *
 * Equivalent to C++ ``std::vector::reserve``.  When the allocation grows, all
 * existing elements are preserved.
 *
 * @param vec Pointer to a vector.
 * @param new_capacity Target capacity requested by the caller.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION on failure.
 * @signature int ac_vector_reserve(ac_vector *vec, size_t new_capacity)
 */
int ac_vector_reserve(ac_vector *vec, size_t new_capacity);

/**
 * @brief Reduce the allocation so that ``capacity == size``.
 *
 * Useful after removing elements in bulk.  Shrinking to zero frees the buffer.
 *
 * @param vec Pointer to a vector.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION if the
 *         reallocation fails.
 * @signature int ac_vector_shrink_to_fit(ac_vector *vec)
 */
int ac_vector_shrink_to_fit(ac_vector *vec);

/**
 * @brief Reset ``size`` to zero without releasing the allocation.
 *
 * Mirrors ``Vector.clear`` from the Python repository.  Future insertions reuse
 * the existing buffer.
 *
 * @param vec Pointer to a vector.
 * @signature void ac_vector_clear(ac_vector *vec)
 */
void ac_vector_clear(ac_vector *vec);

/**
 * @brief Append ``element`` to the logical end of the vector.
 *
 * Amortised ``O(1)`` thanks to the doubling growth policy.  ``element`` is
 * copied into storage.
 *
 * @param vec Pointer to a vector.
 * @param element Address of the value to append (must not be ``NULL``).
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_ALLOCATION on failure.
 * @signature int ac_vector_push_back(ac_vector *vec, const void *element)
 */
int ac_vector_push_back(ac_vector *vec, const void *element);

/**
 * @brief Remove the last element and optionally copy it into ``out_element``.
 *
 * Mirrors ``Vector.pop`` in Python.  Attempting to pop an empty vector returns
 * ::AC_VECTOR_ERR_EMPTY.
 *
 * @param vec Pointer to a vector.
 * @param out_element Optional destination for the removed element.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_EMPTY when the vector is
 *         empty.
 * @signature int ac_vector_pop_back(ac_vector *vec, void *out_element)
 */
int ac_vector_pop_back(ac_vector *vec, void *out_element);

/**
 * @brief Insert ``element`` at position ``index`` shifting subsequent entries.
 *
 * @param vec Pointer to a vector.
 * @param index Position at which to insert (between ``0`` and ``size``).
 * @param element Address of the value to insert.
 * @return ::AC_VECTOR_OK on success, ::AC_VECTOR_ERR_ALLOCATION on allocation
 *         failure, or ::AC_VECTOR_ERR_INDEX when ``index`` is invalid.
 * @signature int ac_vector_insert(ac_vector *vec, size_t index,
 *                                 const void *element)
 */
int ac_vector_insert(ac_vector *vec, size_t index, const void *element);

/**
 * @brief Remove the element located at ``index``.
 *
 * Elements after ``index`` slide left to fill the gap, matching Python list
 * semantics.
 *
 * @param vec Pointer to a vector.
 * @param index Position of the element to remove.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_INDEX when ``index`` is
 *         out of range.
 * @signature int ac_vector_erase(ac_vector *vec, size_t index)
 */
int ac_vector_erase(ac_vector *vec, size_t index);

/**
 * @brief Copy the element at ``index`` into ``out_element``.
 *
 * @param vec Pointer to a vector.
 * @param index Position of the element to retrieve.
 * @param out_element Destination buffer for the value; must not be ``NULL``.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_INDEX otherwise.
 * @signature int ac_vector_get(const ac_vector *vec, size_t index,
 *                              void *out_element)
 */
int ac_vector_get(const ac_vector *vec, size_t index, void *out_element);

/**
 * @brief Replace the element at ``index`` with the value pointed to by
 *        ``element``.
 *
 * @param vec Pointer to a vector.
 * @param index Position of the element to replace.
 * @param element Address of the new value.
 * @return ::AC_VECTOR_OK on success or ::AC_VECTOR_ERR_INDEX otherwise.
 * @signature int ac_vector_set(ac_vector *vec, size_t index,
 *                              const void *element)
 */
int ac_vector_set(ac_vector *vec, size_t index, const void *element);

#ifdef __cplusplus
}
#endif

#endif
