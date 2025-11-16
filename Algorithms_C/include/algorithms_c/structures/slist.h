#ifndef ALGORITHMS_C_STRUCTURES_SLIST_H
#define ALGORITHMS_C_STRUCTURES_SLIST_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file slist.h
 * @brief Verbose single-linked list translation of the Python ``LinkedList``.
 *
 * The original repository dedicates entire docstrings to every public method,
 * describing the invariants, runtime guarantees, and usage examples.  This
 * header mirrors that approach so that the accompanying ``ac_slist`` structure
 * can be read as a tutorial: each declaration begins with a concise summary,
 * follows with a longer explanation, and restates the signature explicitly.
 */

/** Forward declaration of the internal node type (opaque to callers). */
typedef struct ac_slist_node ac_slist_node;

/**
 * @enum ac_slist_result
 * @brief Result codes returned by singly linked list helpers.
 *
 * The constants echo the Python implementation's exceptions: invalid arguments
 * map to ``ValueError`` equivalents, ``IndexError`` arises when positions fall
 * outside the current size, and ``RuntimeError`` analogues appear when the list
 * is empty.
 */
typedef enum {
    /** Operation completed successfully. */
    AC_SLIST_OK = 0,
    /** Arguments were ``NULL`` or element sizes were zero. */
    AC_SLIST_ERR_ARGUMENT = 1,
    /** Memory allocation failed while creating a node. */
    AC_SLIST_ERR_ALLOCATION = 2,
    /** Attempted to read or remove from an empty list. */
    AC_SLIST_ERR_EMPTY = 3,
    /** Requested position is outside ``[0, size)``. */
    AC_SLIST_ERR_INDEX = 4
} ac_slist_result;

/**
 * @struct ac_slist
 * @brief Minimal singly linked list storing fixed-size payloads.
 *
 * ``head`` and ``tail`` point at the first and last nodes respectively, while
 * ``size`` and ``element_size`` track the list length and payload granularity.
 * All nodes own their bytes so that callers can push stack-allocated values
 * without worrying about lifetime management.
 */
typedef struct {
    /** First node in the chain (``NULL`` when the list is empty). */
    ac_slist_node *head;
    /** Last node in the chain (``NULL`` when the list is empty). */
    ac_slist_node *tail;
    /** Number of live elements currently stored. */
    size_t size;
    /** Size in bytes of each stored element. */
    size_t element_size;
} ac_slist;

/** Comparator used by ::ac_slist_contains to evaluate equality. */
typedef int (*ac_slist_compare_fn)(const void *lhs, const void *rhs, void *user_data);

/** Visitor invoked by ::ac_slist_for_each over mutable elements. */
typedef int (*ac_slist_visit_fn)(void *element, void *user_data);

/** Visitor invoked by ::ac_slist_for_each_const over read-only elements. */
typedef int (*ac_slist_const_visit_fn)(const void *element, void *user_data);

/**
 * @brief Initialise an empty singly linked list with fixed ``element_size``.
 *
 * Mirrors ``LinkedList.__init__`` from the Python source: ``head`` and ``tail``
 * start as ``NULL`` sentinels, no allocation occurs until the first push, and
 * the element size becomes immutable for the lifetime of the container.
 *
 * @param list Destination list to initialise.
 * @param element_size Size in bytes of each stored value; must be non-zero.
 * @return ::AC_SLIST_OK on success or ::AC_SLIST_ERR_ARGUMENT when inputs are
 *         invalid.
 * @signature int ac_slist_init(ac_slist *list, size_t element_size)
 */
int ac_slist_init(ac_slist *list, size_t element_size);

/**
 * @brief Destroy ``list`` and release all owned nodes.
 *
 * The function walks the chain, frees each allocation, and resets bookkeeping
 * fields.  After destruction the structure can be re-initialised safely.
 *
 * @param list Pointer to an initialised list (nullable).
 * @signature void ac_slist_destroy(ac_slist *list)
 */
void ac_slist_destroy(ac_slist *list);

/**
 * @brief Remove every element while keeping the structure reusable.
 *
 * Unlike ::ac_slist_destroy the function preserves ``element_size`` so the list
 * can accept new pushes immediately after clearing.
 *
 * @param list Pointer to an initialised list (nullable).
 * @signature void ac_slist_clear(ac_slist *list)
 */
void ac_slist_clear(ac_slist *list);

/**
 * @brief Return how many elements the list currently stores.
 *
 * Equivalent to Python's ``len(list)``.
 *
 * @param list Pointer to an initialised list (nullable).
 * @return Logical size of the list (zero for ``NULL`` pointers).
 * @signature size_t ac_slist_size(const ac_slist *list)
 */
size_t ac_slist_size(const ac_slist *list);

/**
 * @brief Determine whether the list contains zero elements.
 *
 * @param list Pointer to an initialised list (nullable).
 * @return ``true`` when empty, otherwise ``false``.
 * @signature bool ac_slist_empty(const ac_slist *list)
 */
bool ac_slist_empty(const ac_slist *list);

/**
 * @brief Inspect the stored element size.
 *
 * Provided for diagnostics and parity with the Python ``LinkedList`` docstring
 * which explicitly lists the payload granularity.
 *
 * @param list Pointer to an initialised list (nullable).
 * @return Size in bytes of each stored element (zero for ``NULL`` pointers).
 * @signature size_t ac_slist_element_size(const ac_slist *list)
 */
size_t ac_slist_element_size(const ac_slist *list);

/**
 * @brief Append ``value`` to the end of the list.
 *
 * ``value`` is copied into a freshly allocated node so that callers retain
 * ownership of their buffers.  Runtime is ``O(1)`` thanks to the cached tail
 * pointer.
 *
 * @param list Pointer to an initialised list.
 * @param value Address of the element to copy; must not be ``NULL``.
 * @return ::AC_SLIST_OK on success, ::AC_SLIST_ERR_ARGUMENT for invalid inputs,
 *         or ::AC_SLIST_ERR_ALLOCATION if memory runs out.
 * @signature int ac_slist_append(ac_slist *list, const void *value)
 */
int ac_slist_append(ac_slist *list, const void *value);

/**
 * @brief Insert ``value`` at the front of the list.
 *
 * Mirrors ``LinkedList.insert(0, value)`` with a dedicated helper so that
 * callers need not reason about indices when implementing queues.
 *
 * @param list Pointer to an initialised list.
 * @param value Address of the element to copy; must not be ``NULL``.
 * @return ::AC_SLIST_OK on success or an error from ::ac_slist_append.
 * @signature int ac_slist_prepend(ac_slist *list, const void *value)
 */
int ac_slist_prepend(ac_slist *list, const void *value);

/**
 * @brief Insert ``value`` before the element currently stored at ``index``.
 *
 * The operation is ``O(n)`` because singly linked lists must walk the chain to
 * locate the predecessor.  Accepts ``index == size`` to model appends.
 *
 * @param list Pointer to an initialised list.
 * @param index Position at which to insert (0 <= index <= size).
 * @param value Address of the element to copy; must not be ``NULL``.
 * @return ::AC_SLIST_OK on success, ::AC_SLIST_ERR_INDEX when ``index`` is
 *         invalid, ::AC_SLIST_ERR_ARGUMENT for bad inputs, or
 *         ::AC_SLIST_ERR_ALLOCATION on allocation failure.
 * @signature int ac_slist_insert(ac_slist *list, size_t index, const void *value)
 */
int ac_slist_insert(ac_slist *list, size_t index, const void *value);

/**
 * @brief Remove and optionally copy out the first element.
 *
 * Equivalent to ``LinkedList.erase(0)``.  When ``out_value`` is provided the
 * bytes are copied back to the caller before freeing the node.
 *
 * @param list Pointer to an initialised list.
 * @param out_value Optional buffer receiving the removed payload.
 * @return ::AC_SLIST_OK on success or ::AC_SLIST_ERR_EMPTY when the list has no
 *         elements.
 * @signature int ac_slist_pop_front(ac_slist *list, void *out_value)
 */
int ac_slist_pop_front(ac_slist *list, void *out_value);

/**
 * @brief Remove and optionally copy out the last element.
 *
 * Operates in ``O(n)`` time because singly linked lists must walk to the node
 * preceding ``tail``.
 *
 * @param list Pointer to an initialised list.
 * @param out_value Optional buffer receiving the removed payload.
 * @return ::AC_SLIST_OK on success, ::AC_SLIST_ERR_EMPTY when there are no
 *         elements.
 * @signature int ac_slist_pop_back(ac_slist *list, void *out_value)
 */
int ac_slist_pop_back(ac_slist *list, void *out_value);

/**
 * @brief Remove the element at ``index`` and optionally return its payload.
 *
 * This is the generalized form of ::ac_slist_pop_front / ::ac_slist_pop_back.
 *
 * @param list Pointer to an initialised list.
 * @param index Position of the element to delete.
 * @param out_value Optional buffer receiving the removed payload.
 * @return ::AC_SLIST_OK on success, ::AC_SLIST_ERR_INDEX when ``index`` is
 *         invalid, or ::AC_SLIST_ERR_EMPTY if the list stores zero elements.
 * @signature int ac_slist_erase(ac_slist *list, size_t index, void *out_value)
 */
int ac_slist_erase(ac_slist *list, size_t index, void *out_value);

/**
 * @brief Copy the element at ``index`` into ``out_value`` without removal.
 *
 * Provides Python's ``LinkedList.__getitem__`` semantics (``O(n)`` traversal).
 *
 * @param list Pointer to an initialised list.
 * @param index Position of the element to read.
 * @param out_value Destination buffer; must not be ``NULL``.
 * @return ::AC_SLIST_OK on success or ::AC_SLIST_ERR_INDEX otherwise.
 * @signature int ac_slist_get(const ac_slist *list, size_t index, void *out_value)
 */
int ac_slist_get(const ac_slist *list, size_t index, void *out_value);

/**
 * @brief Copy the first element into ``out_value`` without removing it.
 *
 * @param list Pointer to an initialised list.
 * @param out_value Destination buffer; must not be ``NULL``.
 * @return ::AC_SLIST_OK on success or ::AC_SLIST_ERR_EMPTY when the list has no
 *         head node.
 * @signature int ac_slist_front(const ac_slist *list, void *out_value)
 */
int ac_slist_front(const ac_slist *list, void *out_value);

/**
 * @brief Copy the last element into ``out_value`` without removing it.
 *
 * @param list Pointer to an initialised list.
 * @param out_value Destination buffer; must not be ``NULL``.
 * @return ::AC_SLIST_OK on success or ::AC_SLIST_ERR_EMPTY when the list is
 *         empty.
 * @signature int ac_slist_back(const ac_slist *list, void *out_value)
 */
int ac_slist_back(const ac_slist *list, void *out_value);

/**
 * @brief Check whether ``needle`` exists in the list using ``compare``.
 *
 * The function mirrors ``LinkedList.__contains__``: it performs a linear scan
 * and evaluates equality through the caller-provided predicate.
 *
 * @param list Pointer to an initialised list.
 * @param needle Value to search for (passed verbatim to ``compare``).
 * @param compare Comparator returning ``0`` on equality.
 * @param user_data Optional payload forwarded to ``compare``.
 * @return ``true`` when ``needle`` is found, ``false`` for absent elements or
 *         invalid inputs.
 * @signature bool ac_slist_contains(
 *     const ac_slist *list,
 *     const void *needle,
 *     ac_slist_compare_fn compare,
 *     void *user_data)
 */
bool ac_slist_contains(
    const ac_slist *list,
    const void *needle,
    ac_slist_compare_fn compare,
    void *user_data
);

/**
 * @brief Iterate over each element, granting mutable access to ``value``.
 *
 * The visitor decides whether iteration continues: returning ``0`` keeps the
 * traversal going, while any non-zero status aborts early and is forwarded to
 * the caller.  This matches the Python ``list_all`` helper which yielded each
 * node for inspection.
 *
 * @param list Pointer to an initialised list.
 * @param visit Callback invoked for every element; must not be ``NULL``.
 * @param user_data Optional pointer forwarded to ``visit``.
 * @return ::AC_SLIST_OK when the entire list was visited, ``visit``'s return
 *         value when it aborted early, or ::AC_SLIST_ERR_ARGUMENT for bad
 *         inputs.
 * @signature int ac_slist_for_each(ac_slist *list, ac_slist_visit_fn visit,
 *                                  void *user_data)
 */
int ac_slist_for_each(ac_slist *list, ac_slist_visit_fn visit, void *user_data);

/**
 * @brief Iterate over each element in a read-only fashion.
 *
 * Identical to ::ac_slist_for_each but guarantees that the visitor receives a
 * ``const`` pointer.  Useful when verifying contents, as seen in the stress
 * harness.
 *
 * @param list Pointer to an initialised list.
 * @param visit Callback invoked for every element; must not be ``NULL``.
 * @param user_data Optional pointer forwarded to ``visit``.
 * @return ::AC_SLIST_OK on success or an error code as described above.
 * @signature int ac_slist_for_each_const(
 *     const ac_slist *list,
 *     ac_slist_const_visit_fn visit,
 *     void *user_data)
 */
int ac_slist_for_each_const(
    const ac_slist *list,
    ac_slist_const_visit_fn visit,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
