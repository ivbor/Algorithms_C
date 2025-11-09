#include <stdlib.h>
#include <string.h>
#include "algorithms_c/algorithms/sorting.h"

void ac_insertion_sort(
    void *data,
    size_t size,
    size_t element_size,
    ac_compare_fn compare
) {
    if (data == NULL || compare == NULL || element_size == 0) {
        return;
    }

    unsigned char *array = (unsigned char *)data;
    unsigned char *buffer = (unsigned char *)malloc(element_size);
    if (buffer == NULL) {
        return;
    }

    for (size_t i = 1; i < size; ++i) {
        memcpy(buffer, array + (i * element_size), element_size);
        size_t j = i;
        while (j > 0) {
            unsigned char *prev = array + ((j - 1) * element_size);
            if (compare(prev, buffer) <= 0) {
                break;
            }
            memcpy(array + (j * element_size), prev, element_size);
            --j;
        }
        memcpy(array + (j * element_size), buffer, element_size);
    }

    free(buffer);
}
