#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

/**
 * @brief Sort contiguous storage using comb sort with a shrink factor.
 *
 * Source file translated in this step:
 * ``Algorithms_Python/algorithms/sorting/comb_sort.py``.
 *
 * Comb sort improves bubble sort by comparing elements at a configurable gap
 * distance and shrinking that gap on each pass. Once the gap reaches one, the
 * algorithm behaves like a final bubble-sort sweep to finish local inversions.
 * The commonly used shrink factor 1.3 is retained for readability and parity
 * with educational references.
 */
void ac_comb_sort(
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

    const double shrink_factor = 1.3;
    size_t gap = size;
    bool swapped = true;

    while (gap > 1U || swapped) {
        if (gap > 1U) {
            gap = (size_t)((double)gap / shrink_factor);
            if (gap < 1U) {
                gap = 1U;
            }
        }

        swapped = false;

        for (size_t i = 0; i + gap < size; ++i) {
            unsigned char *lhs = array + (i * element_size);
            unsigned char *rhs = array + ((i + gap) * element_size);

            if (compare(lhs, rhs) <= 0) {
                continue;
            }

            memcpy(scratch, lhs, element_size);
            memcpy(lhs, rhs, element_size);
            memcpy(rhs, scratch, element_size);
            swapped = true;
        }
    }

    free(scratch);
}
