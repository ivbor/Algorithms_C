#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "algorithms_c/structures/deque.h"
#include "algorithms_c/structures/queue.h"
#include "algorithms_c/structures/slist.h"
#include "algorithms_c/structures/stack.h"
#include "algorithms_c/structures/vector.h"

#define VECTOR_ITERATIONS 50000
#define DEQUE_ITERATIONS 50000
#define QUEUE_ITERATIONS 50000
#define SLIST_ITERATIONS 50000
#define STACK_ITERATIONS 50000

// Tracks how often each logical operation succeeded versus intentionally
// triggering guard paths (e.g. popping from an empty container).
typedef struct {
    const char *name;
    size_t successes;
    size_t expected_failures;
} operation_stats;

static void print_operation_separator(void) {
    printf("+----------------------+--------------+--------------------+\n");
}

// Render a compact summary similar to the sorting stress harness so that the
// data structure runs expose throughput at a glance.
static void print_operation_table(
    const char *title,
    const operation_stats *stats,
    size_t count,
    size_t iterations,
    double elapsed_ms
) {
    print_operation_separator();
    printf("| %-20s | %12s | %18s |\n", title, "Successes", "Expected empty");
    print_operation_separator();

    size_t total = 0;
    for (size_t i = 0; i < count; ++i) {
        const operation_stats *entry = &stats[i];
        size_t subtotal = entry->successes + entry->expected_failures;
        total += subtotal;
        printf(
            "| %-20s | %12zu | %18zu |\n", entry->name, entry->successes,
            entry->expected_failures
        );
    }

    print_operation_separator();
    double seconds = elapsed_ms > 0.0 ? elapsed_ms / 1000.0 : 0.0;
    double ops_per_sec = seconds > 0.0 ? (double)total / seconds : 0.0;
    printf("Total operations: %zu\n", total);
    printf("Iterations run : %zu\n", iterations);
    printf("Elapsed (ms)   : %.2f\n", elapsed_ms);
    printf("Ops / second   : %.2f\n", ops_per_sec);
    print_operation_separator();
}

static int compare_ints(const void *lhs, const void *rhs, void *user_data) {
    (void)user_data;
    int left = lhs != NULL ? *(const int *)lhs : 0;
    int right = rhs != NULL ? *(const int *)rhs : 0;
    if (left == right) {
        return 0;
    }
    return left < right ? -1 : 1;
}

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} reference_vector;

static int ref_vector_init(reference_vector *ref) {
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
    return 0;
}

static void ref_vector_destroy(reference_vector *ref) {
    free(ref->data);
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static int ref_vector_reserve(reference_vector *ref, size_t new_capacity) {
    if (new_capacity <= ref->capacity) {
        return 0;
    }

    int *new_data = realloc(ref->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return -1;
    }
    ref->data = new_data;
    ref->capacity = new_capacity;
    return 0;
}

static int ref_vector_shrink_to_fit(reference_vector *ref) {
    if (ref->size == ref->capacity) {
        return 0;
    }

    if (ref->size == 0) {
        free(ref->data);
        ref->data = NULL;
        ref->capacity = 0;
        return 0;
    }

    int *new_data = realloc(ref->data, ref->size * sizeof(int));
    if (new_data == NULL) {
        return -1;
    }
    ref->data = new_data;
    ref->capacity = ref->size;
    return 0;
}

static int ref_vector_push_back(reference_vector *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_vector_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    ref->data[ref->size++] = value;
    return 0;
}

static int ref_vector_insert(reference_vector *ref, size_t index, int value) {
    if (index > ref->size) {
        return -1;
    }

    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_vector_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    memmove(
        ref->data + index + 1, ref->data + index,
        (ref->size - index) * sizeof(int)
    );
    ref->data[index] = value;
    ref->size++;
    return 0;
}

static int ref_vector_erase(reference_vector *ref, size_t index) {
    if (index >= ref->size) {
        return -1;
    }

    memmove(
        ref->data + index, ref->data + index + 1,
        (ref->size - index - 1) * sizeof(int)
    );
    ref->size--;
    return 0;
}

static int ref_vector_pop_back(reference_vector *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }

    ref->size--;
    if (out_value != NULL) {
        *out_value = ref->data[ref->size];
    }
    return 0;
}

static void ref_vector_clear(reference_vector *ref) {
    ref->size = 0;
}

static int
verify_vector_matches(const ac_vector *vec, const reference_vector *ref) {
    if (ac_vector_size(vec) != ref->size) {
        fprintf(
            stderr, "Vector size mismatch: %zu vs %zu\n", ac_vector_size(vec),
            ref->size
        );
        return -1;
    }

    for (size_t i = 0; i < ref->size; ++i) {
        int actual = 0;
        if (ac_vector_get(vec, i, &actual) != AC_VECTOR_OK) {
            fprintf(stderr, "Vector get failed at index %zu\n", i);
            return -1;
        }
        if (actual != ref->data[i]) {
            fprintf(
                stderr, "Vector data mismatch at %zu: %d vs %d\n", i, actual,
                ref->data[i]
            );
            return -1;
        }
    }
    return 0;
}

typedef struct {
    const reference_vector *ref;
    size_t index;
} slist_verify_ctx;

static int slist_const_visit(const void *value, void *user_data) {
    slist_verify_ctx *ctx = user_data;
    if (ctx->index >= ctx->ref->size) {
        return AC_SLIST_ERR_INDEX;
    }
    int actual = *(const int *)value;
    int expected = ctx->ref->data[ctx->index++];
    return actual == expected ? 0 : AC_SLIST_ERR_INDEX;
}

static int
verify_slist_matches(const ac_slist *list, const reference_vector *ref) {
    if (ac_slist_size(list) != ref->size) {
        fprintf(
            stderr, "SList size mismatch: %zu vs %zu\n", ac_slist_size(list),
            ref->size
        );
        return -1;
    }

    slist_verify_ctx ctx = {.ref = ref, .index = 0};
    int status = ac_slist_for_each_const(list, slist_const_visit, &ctx);
    if (status != AC_SLIST_OK) {
        fprintf(stderr, "SList data mismatch during verification\n");
        return -1;
    }
    if (ctx.index != ref->size) {
        fprintf(stderr, "SList iteration truncated at index %zu\n", ctx.index);
        return -1;
    }
    return 0;
}

static int run_vector_stress(void) {
    // Use a deterministic reference implementation to cross-check every
    // operation.  This keeps the stress run reproducible while still
    // exercising complex interleavings of operations.
    ac_vector vec;
    reference_vector ref;

    operation_stats stats[] = {
        {"push_back", 0, 0}, {"pop_back", 0, 0}, {"insert", 0, 0},
        {"erase", 0, 0},     {"set", 0, 0},      {"get", 0, 0},
        {"clear", 0, 0},     {"reserve", 0, 0},  {"shrink_to_fit", 0, 0},
    };

    if (ac_vector_init(&vec, sizeof(int)) != AC_VECTOR_OK) {
        fprintf(stderr, "Failed to initialise vector\n");
        return 1;
    }
    ref_vector_init(&ref);

    clock_t start = clock();
    for (size_t iteration = 0; iteration < VECTOR_ITERATIONS; ++iteration) {
        int operation = rand() % 9;
        switch (operation) {
            case 0: {
                int value = rand();
                if (ac_vector_push_back(&vec, &value) != AC_VECTOR_OK ||
                    ref_vector_push_back(&ref, value) != 0) {
                    fprintf(stderr, "Vector push_back failed\n");
                    goto error;
                }
                stats[0].successes++;
                break;
            }
            case 1: {
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_vector_pop_back(&vec, &actual) !=
                        AC_VECTOR_ERR_EMPTY) {
                        fprintf(
                            stderr, "Vector pop_back should report empty\n"
                        );
                        goto error;
                    }
                    stats[1].expected_failures++;
                } else {
                    if (ref_vector_pop_back(&ref, &expected) != 0 ||
                        ac_vector_pop_back(&vec, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Vector pop_back mismatch\n");
                        goto error;
                    }
                    stats[1].successes++;
                }
                break;
            }
            case 2: {
                size_t index =
                    ref.size == 0 ? 0 : (size_t)(rand() % (ref.size + 1));
                int value = rand();
                if (ac_vector_insert(&vec, index, &value) != AC_VECTOR_OK ||
                    ref_vector_insert(&ref, index, value) != 0) {
                    fprintf(stderr, "Vector insert failed\n");
                    goto error;
                }
                stats[2].successes++;
                break;
            }
            case 3: {
                if (ref.size == 0) {
                    if (ac_vector_erase(&vec, 0) != AC_VECTOR_ERR_INDEX) {
                        fprintf(stderr, "Vector erase should fail on empty\n");
                        goto error;
                    }
                    stats[3].expected_failures++;
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    if (ac_vector_erase(&vec, index) != AC_VECTOR_OK ||
                        ref_vector_erase(&ref, index) != 0) {
                        fprintf(stderr, "Vector erase failed\n");
                        goto error;
                    }
                    if (index < ref.size) {
                        int after = 0;
                        if (ac_vector_get(&vec, index, &after) !=
                            AC_VECTOR_OK) {
                            fprintf(stderr, "Vector get after erase failed\n");
                            goto error;
                        }
                        if (ref.data[index] != after) {
                            fprintf(
                                stderr, "Vector data mismatch after erase\n"
                            );
                            goto error;
                        }
                    }
                    stats[3].successes++;
                }
                break;
            }
            case 4: {
                if (ref.size == 0) {
                    int value = rand();
                    if (ac_vector_set(&vec, 0, &value) != AC_VECTOR_ERR_INDEX) {
                        fprintf(stderr, "Vector set should fail on empty\n");
                        goto error;
                    }
                    stats[4].expected_failures++;
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    int value = rand();
                    if (ac_vector_set(&vec, index, &value) != AC_VECTOR_OK) {
                        fprintf(stderr, "Vector set failed\n");
                        goto error;
                    }
                    ref.data[index] = value;
                    stats[4].successes++;
                }
                break;
            }
            case 5: {
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_vector_get(&vec, 0, &actual) !=
                        AC_VECTOR_ERR_INDEX) {
                        fprintf(stderr, "Vector get should fail on empty\n");
                        goto error;
                    }
                    stats[5].expected_failures++;
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    if (ac_vector_get(&vec, index, &actual) != AC_VECTOR_OK ||
                        actual != ref.data[index]) {
                        fprintf(stderr, "Vector get mismatch\n");
                        goto error;
                    }
                    stats[5].successes++;
                }
                break;
            }
            case 6:
                ac_vector_clear(&vec);
                ref_vector_clear(&ref);
                stats[6].successes++;
                break;
            case 7: {
                size_t target = ref.size + (size_t)(rand() % 20);
                if (ac_vector_reserve(&vec, target) != AC_VECTOR_OK ||
                    ref_vector_reserve(&ref, target) != 0) {
                    fprintf(stderr, "Vector reserve failed\n");
                    goto error;
                }
                stats[7].successes++;
                break;
            }
            case 8: {
                if (ac_vector_shrink_to_fit(&vec) != AC_VECTOR_OK ||
                    ref_vector_shrink_to_fit(&ref) != 0) {
                    fprintf(stderr, "Vector shrink_to_fit failed\n");
                    goto error;
                }
                stats[8].successes++;
                break;
            }
        }

        if (ac_vector_size(&vec) != ref.size) {
            fprintf(
                stderr, "Vector size diverged after iteration %zu\n", iteration
            );
            goto error;
        }
    }

    clock_t end = clock();
    double elapsed_ms = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;

    if (verify_vector_matches(&vec, &ref) != 0) {
        goto error;
    }

    ac_vector_destroy(&vec);
    ref_vector_destroy(&ref);
    print_operation_table(
        "Vector operations", stats, sizeof(stats) / sizeof(stats[0]),
        VECTOR_ITERATIONS, elapsed_ms
    );
    printf("Vector stress test passed (%d iterations).\n", VECTOR_ITERATIONS);
    return 0;

error:
    ac_vector_destroy(&vec);
    ref_vector_destroy(&ref);
    return 1;
}

static int slist_mutate_visit(void *value, void *user_data) {
    int delta = *(int *)user_data;
    int *target = value;
    *target += delta;
    return 0;
}

static int run_slist_stress(void) {
    ac_slist list;
    reference_vector ref;

    operation_stats stats[] = {
        {"append", 0, 0},    {"prepend", 0, 0},  {"insert", 0, 0},
        {"pop_front", 0, 0}, {"pop_back", 0, 0}, {"erase", 0, 0},
        {"contains", 0, 0},  {"mutate", 0, 0},   {"clear", 0, 0},
    };

    if (ac_slist_init(&list, sizeof(int)) != AC_SLIST_OK) {
        fprintf(stderr, "Failed to initialise singly linked list\n");
        return 1;
    }
    ref_vector_init(&ref);

    clock_t start = clock();
    for (size_t iteration = 0; iteration < SLIST_ITERATIONS; ++iteration) {
        int operation = rand() % 9;
        switch (operation) {
            case 0: {  // append
                int value = rand();
                if (ac_slist_append(&list, &value) != AC_SLIST_OK ||
                    ref_vector_push_back(&ref, value) != 0) {
                    fprintf(stderr, "SList append failed\n");
                    goto slist_error;
                }
                stats[0].successes++;
                break;
            }
            case 1: {  // prepend
                int value = rand();
                if (ac_slist_prepend(&list, &value) != AC_SLIST_OK ||
                    ref_vector_insert(&ref, 0, value) != 0) {
                    fprintf(stderr, "SList prepend failed\n");
                    goto slist_error;
                }
                stats[1].successes++;
                break;
            }
            case 2: {  // insert random position
                size_t index =
                    ref.size == 0 ? 0 : (size_t)(rand() % (ref.size + 1));
                int value = rand();
                if (ac_slist_insert(&list, index, &value) != AC_SLIST_OK ||
                    ref_vector_insert(&ref, index, value) != 0) {
                    fprintf(stderr, "SList insert failed\n");
                    goto slist_error;
                }
                stats[2].successes++;
                break;
            }
            case 3: {  // pop_front
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_slist_pop_front(&list, &actual) !=
                        AC_SLIST_ERR_EMPTY) {
                        fprintf(
                            stderr, "SList pop_front should report empty\n"
                        );
                        goto slist_error;
                    }
                    stats[3].expected_failures++;
                } else {
                    int expected = ref.data[0];
                    if (ac_slist_pop_front(&list, &actual) != AC_SLIST_OK ||
                        ref_vector_erase(&ref, 0) != 0 || expected != actual) {
                        fprintf(stderr, "SList pop_front mismatch\n");
                        goto slist_error;
                    }
                    stats[3].successes++;
                }
                break;
            }
            case 4: {  // pop_back
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_slist_pop_back(&list, &actual) !=
                        AC_SLIST_ERR_EMPTY) {
                        fprintf(stderr, "SList pop_back should report empty\n");
                        goto slist_error;
                    }
                    stats[4].expected_failures++;
                } else {
                    int expected = 0;
                    if (ref_vector_pop_back(&ref, &expected) != 0 ||
                        ac_slist_pop_back(&list, &actual) != AC_SLIST_OK ||
                        expected != actual) {
                        fprintf(stderr, "SList pop_back mismatch\n");
                        goto slist_error;
                    }
                    stats[4].successes++;
                }
                break;
            }
            case 5: {  // erase arbitrary
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_slist_erase(&list, 0, &actual) !=
                        AC_SLIST_ERR_EMPTY) {
                        fprintf(stderr, "SList erase should report empty\n");
                        goto slist_error;
                    }
                    stats[5].expected_failures++;
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    int expected = ref.data[index];
                    if (ac_slist_erase(&list, index, &actual) != AC_SLIST_OK ||
                        ref_vector_erase(&ref, index) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "SList erase mismatch\n");
                        goto slist_error;
                    }
                    stats[5].successes++;
                }
                break;
            }
            case 6: {  // contains
                int needle = rand();
                bool expected = false;
                for (size_t i = 0; i < ref.size; ++i) {
                    if (ref.data[i] == needle) {
                        expected = true;
                        break;
                    }
                }
                bool actual =
                    ac_slist_contains(&list, &needle, compare_ints, NULL);
                if (actual != expected) {
                    fprintf(stderr, "SList contains diverged\n");
                    goto slist_error;
                }
                stats[6].successes++;
                break;
            }
            case 7: {  // mutate via visitor
                int delta = (rand() % 7) - 3;
                if (ac_slist_for_each(&list, slist_mutate_visit, &delta) !=
                    AC_SLIST_OK) {
                    fprintf(stderr, "SList mutation visitor failed\n");
                    goto slist_error;
                }
                for (size_t i = 0; i < ref.size; ++i) {
                    ref.data[i] += delta;
                }
                stats[7].successes++;
                break;
            }
            case 8:  // clear
                ac_slist_clear(&list);
                ref_vector_clear(&ref);
                stats[8].successes++;
                break;
        }

        if (ac_slist_size(&list) != ref.size) {
            fprintf(
                stderr, "SList size diverged after iteration %zu\n", iteration
            );
            goto slist_error;
        }
    }

    clock_t end = clock();
    double elapsed_ms = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;

    if (verify_slist_matches(&list, &ref) != 0) {
        goto slist_error;
    }

    ac_slist_destroy(&list);
    ref_vector_destroy(&ref);
    print_operation_table(
        "SList operations", stats, sizeof(stats) / sizeof(stats[0]),
        SLIST_ITERATIONS, elapsed_ms
    );
    printf(
        "Singly linked list stress test passed (%d iterations).\n",
        SLIST_ITERATIONS
    );
    return 0;

slist_error:
    ac_slist_destroy(&list);
    ref_vector_destroy(&ref);
    return 1;
}

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} reference_deque;

static void ref_deque_init(reference_deque *ref) {
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static void ref_deque_destroy(reference_deque *ref) {
    free(ref->data);
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static int ref_deque_reserve(reference_deque *ref, size_t new_capacity) {
    if (new_capacity <= ref->capacity) {
        return 0;
    }

    int *new_data = realloc(ref->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return -1;
    }
    ref->data = new_data;
    ref->capacity = new_capacity;
    return 0;
}

static int ref_deque_push_back(reference_deque *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_deque_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    ref->data[ref->size++] = value;
    return 0;
}

static int ref_deque_push_front(reference_deque *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_deque_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    if (ref->size > 0) {
        memmove(ref->data + 1, ref->data, ref->size * sizeof(int));
    }
    ref->data[0] = value;
    ref->size++;
    return 0;
}

static int ref_deque_pop_front(reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }

    if (out_value != NULL) {
        *out_value = ref->data[0];
    }
    if (ref->size > 1) {
        memmove(ref->data, ref->data + 1, (ref->size - 1) * sizeof(int));
    }
    ref->size--;
    return 0;
}

static int ref_deque_pop_back(reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }

    ref->size--;
    if (out_value != NULL) {
        *out_value = ref->data[ref->size];
    }
    return 0;
}

static int ref_deque_front(const reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[0];
    return 0;
}

static int ref_deque_back(const reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[ref->size - 1];
    return 0;
}

static void ref_deque_clear(reference_deque *ref) {
    ref->size = 0;
}

static int run_deque_stress(void) {
    ac_deque deque;
    reference_deque ref;

    operation_stats stats[] = {
        {"push_back", 0, 0}, {"push_front", 0, 0}, {"pop_front", 0, 0},
        {"pop_back", 0, 0},  {"front", 0, 0},      {"back", 0, 0},
        {"clear", 0, 0},     {"reserve", 0, 0},
    };

    if (ac_deque_init(&deque, sizeof(int), 0) != 0) {
        fprintf(stderr, "Failed to initialise deque\n");
        return 1;
    }
    ref_deque_init(&ref);

    clock_t start = clock();
    for (size_t iteration = 0; iteration < DEQUE_ITERATIONS; ++iteration) {
        int operation = rand() % 8;
        switch (operation) {
            case 0: {  // push_back
                int value = rand();
                if (ac_deque_push_back(&deque, &value) != 0 ||
                    ref_deque_push_back(&ref, value) != 0) {
                    fprintf(stderr, "Deque push_back failed\n");
                    goto deque_error;
                }
                stats[0].successes++;
                break;
            }
            case 1: {  // push_front
                int value = rand();
                if (ac_deque_push_front(&deque, &value) != 0 ||
                    ref_deque_push_front(&ref, value) != 0) {
                    fprintf(stderr, "Deque push_front failed\n");
                    goto deque_error;
                }
                stats[1].successes++;
                break;
            }
            case 2: {  // pop_front
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_pop_front(&deque, &actual) != -ENOENT) {
                        fprintf(
                            stderr, "Deque pop_front should report empty\n"
                        );
                        goto deque_error;
                    }
                    stats[2].expected_failures++;
                } else {
                    if (ref_deque_pop_front(&ref, &expected) != 0 ||
                        ac_deque_pop_front(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque pop_front mismatch\n");
                        goto deque_error;
                    }
                    stats[2].successes++;
                }
                break;
            }
            case 3: {  // pop_back
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_pop_back(&deque, &actual) != -ENOENT) {
                        fprintf(stderr, "Deque pop_back should report empty\n");
                        goto deque_error;
                    }
                    stats[3].expected_failures++;
                } else {
                    if (ref_deque_pop_back(&ref, &expected) != 0 ||
                        ac_deque_pop_back(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque pop_back mismatch\n");
                        goto deque_error;
                    }
                    stats[3].successes++;
                }
                break;
            }
            case 4: {  // front
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_front(&deque, &actual) != -ENOENT) {
                        fprintf(stderr, "Deque front should report empty\n");
                        goto deque_error;
                    }
                    stats[4].expected_failures++;
                } else {
                    if (ref_deque_front(&ref, &expected) != 0 ||
                        ac_deque_front(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque front mismatch\n");
                        goto deque_error;
                    }
                    stats[4].successes++;
                }
                break;
            }
            case 5: {  // back
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_back(&deque, &actual) != -ENOENT) {
                        fprintf(stderr, "Deque back should report empty\n");
                        goto deque_error;
                    }
                    stats[5].expected_failures++;
                } else {
                    if (ref_deque_back(&ref, &expected) != 0 ||
                        ac_deque_back(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque back mismatch\n");
                        goto deque_error;
                    }
                    stats[5].successes++;
                }
                break;
            }
            case 6:  // clear
                ac_deque_clear(&deque);
                ref_deque_clear(&ref);
                stats[6].successes++;
                break;
            case 7: {  // reserve
                size_t target = ref.size + (size_t)(rand() % 20);
                if (ac_deque_reserve(&deque, target) != 0 ||
                    ref_deque_reserve(&ref, target) != 0) {
                    fprintf(stderr, "Deque reserve failed\n");
                    goto deque_error;
                }
                stats[7].successes++;
                break;
            }
        }

        if (ac_deque_size(&deque) != ref.size) {
            fprintf(
                stderr, "Deque size diverged after iteration %zu\n", iteration
            );
            goto deque_error;
        }
    }

    clock_t end = clock();
    double elapsed_ms = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;

    while (ref.size > 0) {
        int expected = 0;
        int actual = 0;
        if (ref_deque_pop_front(&ref, &expected) != 0 ||
            ac_deque_pop_front(&deque, &actual) != 0 || expected != actual) {
            fprintf(stderr, "Deque final drain mismatch\n");
            goto deque_error;
        }
        stats[2].successes++;
    }

    ac_deque_destroy(&deque);
    ref_deque_destroy(&ref);
    print_operation_table(
        "Deque operations", stats, sizeof(stats) / sizeof(stats[0]),
        DEQUE_ITERATIONS, elapsed_ms
    );
    printf("Deque stress test passed (%d iterations).\n", DEQUE_ITERATIONS);
    return 0;

deque_error:
    ac_deque_destroy(&deque);
    ref_deque_destroy(&ref);
    return 1;
}

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} reference_deque;

static void ref_deque_init(reference_deque *ref) {
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static void ref_deque_destroy(reference_deque *ref) {
    free(ref->data);
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static int ref_deque_reserve(reference_deque *ref, size_t new_capacity) {
    if (new_capacity <= ref->capacity) {
        return 0;
    }

    int *new_data = realloc(ref->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return -1;
    }
    ref->data = new_data;
    ref->capacity = new_capacity;
    return 0;
}

static int ref_deque_push_back(reference_deque *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_deque_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    ref->data[ref->size++] = value;
    return 0;
}

static int ref_deque_push_front(reference_deque *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_deque_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    if (ref->size > 0) {
        memmove(ref->data + 1, ref->data, ref->size * sizeof(int));
    }
    ref->data[0] = value;
    ref->size++;
    return 0;
}

static int ref_deque_pop_front(reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }

    if (out_value != NULL) {
        *out_value = ref->data[0];
    }
    if (ref->size > 1) {
        memmove(ref->data, ref->data + 1, (ref->size - 1) * sizeof(int));
    }
    ref->size--;
    return 0;
}

static int ref_deque_pop_back(reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }

    ref->size--;
    if (out_value != NULL) {
        *out_value = ref->data[ref->size];
    }
    return 0;
}

static int ref_deque_front(const reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[0];
    return 0;
}

static int ref_deque_back(const reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[ref->size - 1];
    return 0;
}

static void ref_deque_clear(reference_deque *ref) {
    ref->size = 0;
}

static int run_deque_stress(void) {
    ac_deque deque;
    reference_deque ref;

    operation_stats stats[] = {
        {"push_back", 0, 0}, {"push_front", 0, 0}, {"pop_front", 0, 0},
        {"pop_back", 0, 0},  {"front", 0, 0},      {"back", 0, 0},
        {"clear", 0, 0},     {"reserve", 0, 0},
    };

    if (ac_deque_init(&deque, sizeof(int), 0) != 0) {
        fprintf(stderr, "Failed to initialise deque\n");
        return 1;
    }
    ref_deque_init(&ref);

    clock_t start = clock();
    for (size_t iteration = 0; iteration < DEQUE_ITERATIONS; ++iteration) {
        int operation = rand() % 8;
        switch (operation) {
            case 0: {  // push_back
                int value = rand();
                if (ac_deque_push_back(&deque, &value) != 0 ||
                    ref_deque_push_back(&ref, value) != 0) {
                    fprintf(stderr, "Deque push_back failed\n");
                    goto deque_error;
                }
                stats[0].successes++;
                break;
            }
            case 1: {  // push_front
                int value = rand();
                if (ac_deque_push_front(&deque, &value) != 0 ||
                    ref_deque_push_front(&ref, value) != 0) {
                    fprintf(stderr, "Deque push_front failed\n");
                    goto deque_error;
                }
                stats[1].successes++;
                break;
            }
            case 2: {  // pop_front
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_pop_front(&deque, &actual) != -ENOENT) {
                        fprintf(
                            stderr, "Deque pop_front should report empty\n"
                        );
                        goto deque_error;
                    }
                    stats[2].expected_failures++;
                } else {
                    if (ref_deque_pop_front(&ref, &expected) != 0 ||
                        ac_deque_pop_front(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque pop_front mismatch\n");
                        goto deque_error;
                    }
                    stats[2].successes++;
                }
                break;
            }
            case 3: {  // pop_back
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_pop_back(&deque, &actual) != -ENOENT) {
                        fprintf(stderr, "Deque pop_back should report empty\n");
                        goto deque_error;
                    }
                    stats[3].expected_failures++;
                } else {
                    if (ref_deque_pop_back(&ref, &expected) != 0 ||
                        ac_deque_pop_back(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque pop_back mismatch\n");
                        goto deque_error;
                    }
                    stats[3].successes++;
                }
                break;
            }
            case 4: {  // front
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_front(&deque, &actual) != -ENOENT) {
                        fprintf(stderr, "Deque front should report empty\n");
                        goto deque_error;
                    }
                    stats[4].expected_failures++;
                } else {
                    if (ref_deque_front(&ref, &expected) != 0 ||
                        ac_deque_front(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque front mismatch\n");
                        goto deque_error;
                    }
                    stats[4].successes++;
                }
                break;
            }
            case 5: {  // back
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_deque_back(&deque, &actual) != -ENOENT) {
                        fprintf(stderr, "Deque back should report empty\n");
                        goto deque_error;
                    }
                    stats[5].expected_failures++;
                } else {
                    if (ref_deque_back(&ref, &expected) != 0 ||
                        ac_deque_back(&deque, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Deque back mismatch\n");
                        goto deque_error;
                    }
                    stats[5].successes++;
                }
                break;
            }
            case 6:  // clear
                ac_deque_clear(&deque);
                ref_deque_clear(&ref);
                stats[6].successes++;
                break;
            case 7: {  // reserve
                size_t target = ref.size + (size_t)(rand() % 20);
                if (ac_deque_reserve(&deque, target) != 0 ||
                    ref_deque_reserve(&ref, target) != 0) {
                    fprintf(stderr, "Deque reserve failed\n");
                    goto deque_error;
                }
                stats[7].successes++;
                break;
            }
        }

        if (ac_deque_size(&deque) != ref.size) {
            fprintf(
                stderr, "Deque size diverged after iteration %zu\n", iteration
            );
            goto deque_error;
        }
    }

    clock_t end = clock();
    double elapsed_ms = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;

    while (ref.size > 0) {
        int expected = 0;
        int actual = 0;
        if (ref_deque_pop_front(&ref, &expected) != 0 ||
            ac_deque_pop_front(&deque, &actual) != 0 || expected != actual) {
            fprintf(stderr, "Deque final drain mismatch\n");
            goto deque_error;
        }
        stats[2].successes++;
    }

    ac_deque_destroy(&deque);
    ref_deque_destroy(&ref);
    print_operation_table(
        "Deque operations", stats, sizeof(stats) / sizeof(stats[0]),
        DEQUE_ITERATIONS, elapsed_ms
    );
    printf("Deque stress test passed (%d iterations).\n", DEQUE_ITERATIONS);
    return 0;

deque_error:
    ac_deque_destroy(&deque);
    ref_deque_destroy(&ref);
    return 1;
}

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} reference_queue;

static void ref_queue_init(reference_queue *ref) {
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static void ref_queue_destroy(reference_queue *ref) {
    free(ref->data);
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static int ref_queue_reserve(reference_queue *ref, size_t new_capacity) {
    if (new_capacity <= ref->capacity) {
        return 0;
    }

    int *new_data = realloc(ref->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return -1;
    }
    ref->data = new_data;
    ref->capacity = new_capacity;
    return 0;
}

static int ref_queue_enqueue(reference_queue *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_queue_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }
    ref->data[ref->size++] = value;
    return 0;
}

static int ref_queue_dequeue(reference_queue *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    if (out_value != NULL) {
        *out_value = ref->data[0];
    }
    memmove(ref->data, ref->data + 1, (ref->size - 1) * sizeof(int));
    ref->size--;
    return 0;
}

static int ref_queue_peek(const reference_queue *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[0];
    return 0;
}

static int run_queue_stress(void) {
    // The queue stressor mirrors the vector approach: a mirror data structure
    // keeps the authoritative state so that we detect even subtle divergence.
    ac_queue queue;
    reference_queue ref;

    operation_stats stats[] = {
        {"enqueue", 0, 0}, {"dequeue", 0, 0},     {"peek", 0, 0},
        {"reserve", 0, 0}, {"consistency", 0, 0},
    };

    if (ac_queue_init(&queue, sizeof(int), 0) != 0) {
        fprintf(stderr, "Failed to initialise queue\n");
        return 1;
    }
    ref_queue_init(&ref);

    clock_t start = clock();
    for (size_t iteration = 0; iteration < QUEUE_ITERATIONS; ++iteration) {
        int operation = rand() % 5;
        switch (operation) {
            case 0: {
                int value = rand();
                if (ac_queue_enqueue(&queue, &value) != 0 ||
                    ref_queue_enqueue(&ref, value) != 0) {
                    fprintf(stderr, "Queue enqueue failed\n");
                    goto queue_error;
                }
                stats[0].successes++;
                break;
            }
            case 1: {
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_queue_dequeue(&queue, &actual) != -ENOENT) {
                        fprintf(stderr, "Queue dequeue should report empty\n");
                        goto queue_error;
                    }
                    stats[1].expected_failures++;
                } else {
                    if (ref_queue_dequeue(&ref, &expected) != 0 ||
                        ac_queue_dequeue(&queue, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Queue dequeue mismatch\n");
                        goto queue_error;
                    }
                    stats[1].successes++;
                }
                break;
            }
            case 2: {
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_queue_peek(&queue, &actual) != -ENOENT) {
                        fprintf(stderr, "Queue peek should report empty\n");
                        goto queue_error;
                    }
                    stats[2].expected_failures++;
                } else {
                    if (ref_queue_peek(&ref, &expected) != 0 ||
                        ac_queue_peek(&queue, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Queue peek mismatch\n");
                        goto queue_error;
                    }
                    stats[2].successes++;
                }
                break;
            }
            case 3: {
                size_t target = ref.size + (size_t)(rand() % 20);
                if (ac_queue_reserve(&queue, target) != 0 ||
                    ref_queue_reserve(&ref, target) != 0) {
                    fprintf(stderr, "Queue reserve failed\n");
                    goto queue_error;
                }
                stats[3].successes++;
                break;
            }
            case 4: {
                if (ref.size > 0 && (rand() % 5) == 0) {
                    int expected = 0;
                    int actual = 0;
                    if (ref_queue_peek(&ref, &expected) != 0 ||
                        ac_queue_peek(&queue, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Queue consistency check failed\n");
                        goto queue_error;
                    }
                    stats[4].successes++;
                }
                break;
            }
        }

        if (ac_queue_size(&queue) != ref.size) {
            fprintf(
                stderr, "Queue size diverged after iteration %zu\n", iteration
            );
            goto queue_error;
        }
    }

    clock_t end = clock();
    double elapsed_ms = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;

    while (ref.size > 0) {
        int expected = 0;
        int actual = 0;
        if (ref_queue_dequeue(&ref, &expected) != 0 ||
            ac_queue_dequeue(&queue, &actual) != 0 || expected != actual) {
            fprintf(stderr, "Queue final drain mismatch\n");
            goto queue_error;
        }
        stats[1].successes++;
    }

    ac_queue_destroy(&queue);
    ref_queue_destroy(&ref);
    print_operation_table(
        "Queue operations", stats, sizeof(stats) / sizeof(stats[0]),
        QUEUE_ITERATIONS, elapsed_ms
    );
    printf("Queue stress test passed (%d iterations).\n", QUEUE_ITERATIONS);
    return 0;

queue_error:
    ac_queue_destroy(&queue);
    ref_queue_destroy(&ref);
    return 1;
}

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} reference_stack;

static void ref_stack_init(reference_stack *ref) {
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static void ref_stack_destroy(reference_stack *ref) {
    free(ref->data);
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static int ref_stack_reserve(reference_stack *ref, size_t new_capacity) {
    if (new_capacity <= ref->capacity) {
        return 0;
    }
    int *new_data = realloc(ref->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return -1;
    }
    ref->data = new_data;
    ref->capacity = new_capacity;
    return 0;
}

static int ref_stack_push(reference_stack *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_stack_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }
    ref->data[ref->size++] = value;
    return 0;
}

static int ref_stack_pop(reference_stack *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    ref->size--;
    if (out_value != NULL) {
        *out_value = ref->data[ref->size];
    }
    return 0;
}

static int ref_stack_top(const reference_stack *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[ref->size - 1];
    return 0;
}

static int run_stack_stress(void) {
    // Stress the stack by randomly interleaving pushes/pops and validating
    // against a simple reference implementation.
    ac_stack stack;
    reference_stack ref;

    operation_stats stats[] = {
        {"push", 0, 0},
        {"pop", 0, 0},
        {"top", 0, 0},
        {"consistency", 0, 0},
    };

    if (ac_stack_init(&stack, sizeof(int)) != AC_VECTOR_OK) {
        fprintf(stderr, "Failed to initialise stack\n");
        return 1;
    }
    ref_stack_init(&ref);

    clock_t start = clock();
    for (size_t iteration = 0; iteration < STACK_ITERATIONS; ++iteration) {
        int operation = rand() % 4;
        switch (operation) {
            case 0: {
                int value = rand();
                if (ac_stack_push(&stack, &value) != AC_VECTOR_OK ||
                    ref_stack_push(&ref, value) != 0) {
                    fprintf(stderr, "Stack push failed\n");
                    goto stack_error;
                }
                stats[0].successes++;
                break;
            }
            case 1: {
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_stack_pop(&stack, &actual) != AC_VECTOR_ERR_EMPTY) {
                        fprintf(stderr, "Stack pop should report empty\n");
                        goto stack_error;
                    }
                    stats[1].expected_failures++;
                } else {
                    if (ref_stack_pop(&ref, &expected) != 0 ||
                        ac_stack_pop(&stack, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Stack pop mismatch\n");
                        goto stack_error;
                    }
                    stats[1].successes++;
                }
                break;
            }
            case 2: {
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_stack_top(&stack, &actual) != AC_VECTOR_ERR_EMPTY) {
                        fprintf(stderr, "Stack top should report empty\n");
                        goto stack_error;
                    }
                    stats[2].expected_failures++;
                } else {
                    if (ref_stack_top(&ref, &expected) != 0 ||
                        ac_stack_top(&stack, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Stack top mismatch\n");
                        goto stack_error;
                    }
                    stats[2].successes++;
                }
                break;
            }
            case 3:
                if (ref.size > 0 && (rand() % 5) == 0) {
                    int expected = 0;
                    int actual = 0;
                    if (ref_stack_top(&ref, &expected) != 0 ||
                        ac_stack_top(&stack, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Stack consistency check failed\n");
                        goto stack_error;
                    }
                    stats[3].successes++;
                }
                break;
        }

        if (ac_stack_size(&stack) != ref.size) {
            fprintf(
                stderr, "Stack size diverged after iteration %zu\n", iteration
            );
            goto stack_error;
        }
    }

    clock_t end = clock();
    double elapsed_ms = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;

    while (ref.size > 0) {
        int expected = 0;
        int actual = 0;
        if (ref_stack_pop(&ref, &expected) != 0 ||
            ac_stack_pop(&stack, &actual) != AC_VECTOR_OK ||
            expected != actual) {
            fprintf(stderr, "Stack final drain mismatch\n");
            goto stack_error;
        }
        stats[1].successes++;
    }

    ac_stack_destroy(&stack);
    ref_stack_destroy(&ref);
    print_operation_table(
        "Stack operations", stats, sizeof(stats) / sizeof(stats[0]),
        STACK_ITERATIONS, elapsed_ms
    );
    printf("Stack stress test passed (%d iterations).\n", STACK_ITERATIONS);
    return 0;

stack_error:
    ac_stack_destroy(&stack);
    ref_stack_destroy(&ref);
    return 1;
}

int main(void) {
    srand((unsigned int)time(NULL));

    if (run_vector_stress() != 0) {
        return 1;
    }
    if (run_slist_stress() != 0) {
        return 1;
    }
    if (run_queue_stress() != 0) {
        return 1;
    }
    if (run_deque_stress() != 0) {
        return 1;
    }
    if (run_stack_stress() != 0) {
        return 1;
    }

    printf("All structure stress tests completed successfully.\n");
    return 0;
}
