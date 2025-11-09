#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "algorithms_c/algorithms/compare.h"
#include "algorithms_c/algorithms/sorting.h"

static const size_t kTrials = 3;

typedef void (*generic_sort_fn)(void *, size_t, size_t, ac_compare_fn);
typedef void (*counting_sort_fn)(int *, size_t, int, int);

typedef enum { SORT_GENERIC, SORT_COUNTING } sort_kind;

typedef struct {
    const char *name;
    sort_kind kind;
    size_t max_size;

    union {
        generic_sort_fn generic;
        counting_sort_fn counting;
    } fn;
} sort_entry;

static const sort_entry kSorts[] = {
    {"Insertion", SORT_GENERIC, 20000, {.generic = ac_insertion_sort}},
    {"Merge", SORT_GENERIC, 200000, {.generic = ac_merge_sort}},
    {"Quick", SORT_GENERIC, 200000, {.generic = ac_quick_sort}},
    {"Counting", SORT_COUNTING, 300000, {.counting = ac_counting_sort_int}},
};

static const size_t kSizes[] = {10000, 50000, 100000, 200000};

static int is_sorted(const int *data, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        if (data[i - 1] > data[i]) {
            return 0;
        }
    }
    return 1;
}

static void print_separator(size_t sort_count) {
    printf("+------------");
    for (size_t i = 0; i < sort_count; ++i) {
        printf("+----------------");
    }
    printf("+\n");
}

static void print_header(size_t sort_count) {
    print_separator(sort_count);
    printf("| %10s ", "Size");
    for (size_t i = 0; i < sort_count; ++i) {
        char header[32];
        snprintf(header, sizeof(header), "%s (ms)", kSorts[i].name);
        printf("| %14s ", header);
    }
    printf("|\n");
    print_separator(sort_count);
}

static void fill_random(int *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        data[i] = (int)(rand() % (int)(size * 2 + 1));
    }
}

static double
benchmark_generic(generic_sort_fn fn, const int *base, int *work, size_t size) {
    double total_ms = 0.0;
    for (size_t trial = 0; trial < kTrials; ++trial) {
        memcpy(work, base, size * sizeof(int));
        clock_t start = clock();
        fn(work, size, sizeof(int), ac_compare_int);
        clock_t end = clock();
        total_ms += (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;
        if (!is_sorted(work, size)) {
            return -1.0;
        }
    }
    return total_ms / (double)kTrials;
}

static double benchmark_counting(
    counting_sort_fn fn,
    const int *base,
    int *work,
    size_t size
) {
    double total_ms = 0.0;
    int min_value = base[0];
    int max_value = base[0];
    for (size_t i = 1; i < size; ++i) {
        if (base[i] < min_value) {
            min_value = base[i];
        }
        if (base[i] > max_value) {
            max_value = base[i];
        }
    }

    for (size_t trial = 0; trial < kTrials; ++trial) {
        memcpy(work, base, size * sizeof(int));
        clock_t start = clock();
        fn(work, size, min_value, max_value);
        clock_t end = clock();
        total_ms += (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;
        if (!is_sorted(work, size)) {
            return -1.0;
        }
    }
    return total_ms / (double)kTrials;
}

int main(void) {
    const size_t sort_count = sizeof(kSorts) / sizeof(kSorts[0]);
    const size_t size_count = sizeof(kSizes) / sizeof(kSizes[0]);

    size_t max_size = 0;
    for (size_t i = 0; i < size_count; ++i) {
        if (kSizes[i] > max_size) {
            max_size = kSizes[i];
        }
    }

    int *base = (int *)malloc(max_size * sizeof(int));
    int *work = (int *)malloc(max_size * sizeof(int));
    if (base == NULL || work == NULL) {
        fprintf(stderr, "Failed to allocate buffers\n");
        free(base);
        free(work);
        return 1;
    }

    srand(42u);

    print_header(sort_count);
    for (size_t index = 0; index < size_count; ++index) {
        size_t size = kSizes[index];
        fill_random(base, size);

        printf("| %10zu ", size);
        for (size_t sort_index = 0; sort_index < sort_count; ++sort_index) {
            const sort_entry *entry = &kSorts[sort_index];
            if (size > entry->max_size) {
                printf("| %14s ", "N/A");
                continue;
            }

            double result = -1.0;
            if (entry->kind == SORT_GENERIC) {
                result = benchmark_generic(entry->fn.generic, base, work, size);
            } else {
                result =
                    benchmark_counting(entry->fn.counting, base, work, size);
            }

            if (result < 0.0) {
                fprintf(
                    stderr, "Sorting failed for %s at size %zu\n", entry->name,
                    size
                );
                free(base);
                free(work);
                return 1;
            }

            printf("| %14.2f ", result);
        }
        printf("|\n");
    }
    print_separator(sort_count);

    free(base);
    free(work);
    printf("Stress benchmarking completed successfully.\n");
    return 0;
}
