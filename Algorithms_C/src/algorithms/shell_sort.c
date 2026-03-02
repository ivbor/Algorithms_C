#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

/**
 * @brief Sort contiguous storage with Shell sort using gap-based insertion.
 *
 * This routine mirrors a common educational Python variant of Shell sort:
 * repeatedly perform insertion-sort-like passes over elements spaced by a
 * shrinking ``gap``. The initial gap is ``size / 2`` and is halved until the
 * final pass uses ``gap == 1``. That last pass corresponds to insertion sort,
 * but on data that is already partially ordered by earlier gap passes.
 *
 * The implementation keeps generic element support by moving values via a
 * temporary buffer and comparator callback.
 */
void ac_shell_sort(
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

    // Gap sequence: N/2, N/4, ..., 1. This simple sequence keeps the
    // implementation easy to follow and close to common textbook examples.
    for (size_t gap = size / 2U; gap > 0; gap /= 2U) {
        for (size_t i = gap; i < size; ++i) {
            memcpy(buffer, array + (i * element_size), element_size);

            size_t j = i;
            while (j >= gap) {
                unsigned char *previous = array + ((j - gap) * element_size);
                if (compare(previous, buffer) <= 0) {
                    break;
                }
                memcpy(array + (j * element_size), previous, element_size);
                j -= gap;
            }

            memcpy(array + (j * element_size), buffer, element_size);
        }
    }

    free(buffer);
}
