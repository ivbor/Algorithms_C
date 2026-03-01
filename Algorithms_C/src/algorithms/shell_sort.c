#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

/*
 * Shell sort translation using Knuth's gap sequence: 1, 4, 13, 40, ...
 * The algorithm performs gapped insertion passes that progressively reduce the
 * distance between compared elements until a final gap of 1 completes sorting.
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

    size_t gap = 1;
    while (gap < size / 3) {
        gap = (3 * gap) + 1;
    }

    while (gap >= 1) {
        for (size_t i = gap; i < size; ++i) {
            memcpy(buffer, array + (i * element_size), element_size);
            size_t j = i;
            while (j >= gap) {
                unsigned char *left = array + ((j - gap) * element_size);
                if (compare(left, buffer) <= 0) {
                    break;
                }
                memcpy(array + (j * element_size), left, element_size);
                j -= gap;
            }
            memcpy(array + (j * element_size), buffer, element_size);
        }

        if (gap == 1) {
            break;
        }
        gap /= 3;
    }

    free(buffer);
}
