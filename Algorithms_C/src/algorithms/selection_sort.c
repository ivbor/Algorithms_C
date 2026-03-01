#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

/**
 * @brief Sort contiguous storage using the selection-sort strategy.
 *
 * Selection sort mirrors the educational Python implementation: for each
 * position ``i`` it searches the unsorted suffix ``[i, n)`` for the smallest
 * element and swaps that element into place. The algorithm intentionally does
 * not preserve stability, but it performs only ``n - 1`` swaps, which makes it
 * a useful didactic contrast against insertion sort and merge sort.
 *
 * The implementation uses a single temporary buffer sized to one element and
 * relies on the caller-provided comparator to preserve generic type support.
 */
void ac_selection_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    if (data == NULL || compare == NULL || element_size == 0 || size < 2) {
        return;
    }

    unsigned char *array = (unsigned char *)data;
    unsigned char *buffer = (unsigned char *)malloc(element_size);
    if (buffer == NULL) {
        return;
    }

    // Grow a sorted prefix left-to-right by selecting the minimum from the
    // remaining suffix and swapping it into the current front position.
    for (size_t i = 0; i + 1 < size; ++i) {
        size_t min_index = i;

        for (size_t j = i + 1; j < size; ++j) {
            unsigned char *current = array + (j * element_size);
            unsigned char *current_min = array + (min_index * element_size);
            if (compare(current, current_min) < 0) {
                min_index = j;
            }
        }

        if (min_index == i) {
            continue;
        }

        unsigned char *lhs = array + (i * element_size);
        unsigned char *rhs = array + (min_index * element_size);
        memcpy(buffer, lhs, element_size);
        memcpy(lhs, rhs, element_size);
        memcpy(rhs, buffer, element_size);
    }

    free(buffer);
}
