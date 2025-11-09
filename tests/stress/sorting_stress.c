#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algorithms_c/algorithms/binary_search.h"
#include "algorithms_c/algorithms/sorting.h"

static void shuffle(int *data, size_t size) {
    for (size_t i = size - 1; i > 0; --i) {
        size_t j = (size_t)(rand() % (int)(i + 1));
        int tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }
}

static int is_sorted(const int *data, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        if (data[i - 1] > data[i]) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    const size_t size = 1000;
    int *data = (int *)malloc(size * sizeof(int));
    int *copy = (int *)malloc(size * sizeof(int));
    if (data == NULL || copy == NULL) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    for (size_t iteration = 0; iteration < 200; ++iteration) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = (int)i;
            copy[i] = (int)i;
        }
        shuffle(data, size);
        shuffle(copy, size);

        ac_quick_sort(data, size, sizeof(int), ac_compare_int);
        ac_merge_sort(copy, size, sizeof(int), ac_compare_int);

        if (!is_sorted(data, size) || !is_sorted(copy, size)) {
            fprintf(stderr, "Sorting failed on iteration %zu\n", iteration);
            free(data);
            free(copy);
            return 1;
        }
    }

    free(data);
    free(copy);
    printf("Stress tests passed!\n");
    return 0;
}
