#include <errno.h>
#include <stddef.h>
#include "algorithms_c/algorithms/sorting.h"
#include "algorithms_c/structures/heap.h"

void ac_heap_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    /* Defensive programming: mirror Python's expectation that inputs are valid.
     */
    if (data == NULL || element_size == 0 || compare == NULL || size == 0) {
        return;
    }

    ac_heap heap;
    if (ac_heap_with_capacity(&heap, element_size, size, compare) != 0) {
        return;
    }

    unsigned char *bytes = (unsigned char *)data;
    size_t index = 0U;
    for (index = 0U; index < size; ++index) {
        if (ac_heap_push(&heap, bytes + (index * element_size)) != 0) {
            ac_heap_destroy(&heap);
            return;
        }
    }

    for (index = 0U; index < size; ++index) {
        if (ac_heap_pop(&heap, bytes + (index * element_size)) != 0) {
            break;
        }
    }

    ac_heap_destroy(&heap);
}
