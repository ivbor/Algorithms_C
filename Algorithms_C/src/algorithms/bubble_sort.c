#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

/**
 * @brief Sort contiguous storage using the classic bubble-sort pass strategy.
 *
 * This translation keeps the Python teaching intent explicit: adjacent elements
 * are compared and swapped until the largest value in the unsorted prefix
 * "bubbles" toward the end of the array after each pass. The implementation
 * includes an early-exit optimization (`swapped` flag), which preserves the
 * same asymptotic worst case (O(n^2)) while allowing nearly sorted inputs to
 * terminate early.
 *
 * A single scratch buffer is reused for all swaps to keep the routine generic
 * across element types without allocating per comparison.
 */
void ac_bubble_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    if (data == NULL || compare == NULL || element_size == 0 || size < 2) {
        return;
    }

    unsigned char *array = (unsigned char *)data;
    unsigned char *scratch = (unsigned char *)malloc(element_size);
    if (scratch == NULL) {
        return;
    }

    // After each outer pass, the largest element in the remaining unsorted
    // prefix is guaranteed to settle at position `pass_end - 1`.
    for (size_t pass_end = size; pass_end > 1; --pass_end) {
        bool swapped = false;

        for (size_t i = 1; i < pass_end; ++i) {
            unsigned char *lhs = array + ((i - 1U) * element_size);
            unsigned char *rhs = array + (i * element_size);
            if (compare(lhs, rhs) <= 0) {
                continue;
            }

            memcpy(scratch, lhs, element_size);
            memcpy(lhs, rhs, element_size);
            memcpy(rhs, scratch, element_size);
            swapped = true;
        }

        if (!swapped) {
            break;
        }
    }

    free(scratch);
}
