#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "algorithms_c/structures/queue.h"
#include "algorithms_c/structures/stack.h"
#include "algorithms_c/structures/vector.h"

#define VECTOR_ITERATIONS 50000
#define QUEUE_ITERATIONS 50000
#define STACK_ITERATIONS 50000

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
        ref->data + index + 1,
        ref->data + index,
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
        ref->data + index,
        ref->data + index + 1,
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

static int verify_vector_matches(const ac_vector *vec, const reference_vector *ref) {
    if (ac_vector_size(vec) != ref->size) {
        fprintf(stderr, "Vector size mismatch: %zu vs %zu\n", ac_vector_size(vec), ref->size);
        return -1;
    }

    for (size_t i = 0; i < ref->size; ++i) {
        int actual = 0;
        if (ac_vector_get(vec, i, &actual) != AC_VECTOR_OK) {
            fprintf(stderr, "Vector get failed at index %zu\n", i);
            return -1;
        }
        if (actual != ref->data[i]) {
            fprintf(stderr, "Vector data mismatch at %zu: %d vs %d\n", i, actual, ref->data[i]);
            return -1;
        }
    }
    return 0;
}

static int run_vector_stress(void) {
    ac_vector vec;
    reference_vector ref;

    if (ac_vector_init(&vec, sizeof(int)) != AC_VECTOR_OK) {
        fprintf(stderr, "Failed to initialise vector\n");
        return 1;
    }
    ref_vector_init(&ref);

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
                break;
            }
            case 1: {
                int expected = 0;
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_vector_pop_back(&vec, &actual) != AC_VECTOR_ERR_EMPTY) {
                        fprintf(stderr, "Vector pop_back should report empty\n");
                        goto error;
                    }
                } else {
                    if (ref_vector_pop_back(&ref, &expected) != 0 ||
                        ac_vector_pop_back(&vec, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Vector pop_back mismatch\n");
                        goto error;
                    }
                }
                break;
            }
            case 2: {
                size_t index = ref.size == 0 ? 0 : (size_t)(rand() % (ref.size + 1));
                int value = rand();
                if (ac_vector_insert(&vec, index, &value) != AC_VECTOR_OK ||
                    ref_vector_insert(&ref, index, value) != 0) {
                    fprintf(stderr, "Vector insert failed\n");
                    goto error;
                }
                break;
            }
            case 3: {
                if (ref.size == 0) {
                    if (ac_vector_erase(&vec, 0) != AC_VECTOR_ERR_INDEX) {
                        fprintf(stderr, "Vector erase should fail on empty\n");
                        goto error;
                    }
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    if (ac_vector_erase(&vec, index) != AC_VECTOR_OK ||
                        ref_vector_erase(&ref, index) != 0) {
                        fprintf(stderr, "Vector erase failed\n");
                        goto error;
                    }
                    if (index < ref.size) {
                        int after = 0;
                        if (ac_vector_get(&vec, index, &after) != AC_VECTOR_OK) {
                            fprintf(stderr, "Vector get after erase failed\n");
                            goto error;
                        }
                        if (ref.data[index] != after) {
                            fprintf(stderr, "Vector data mismatch after erase\n");
                            goto error;
                        }
                    }
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
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    int value = rand();
                    if (ac_vector_set(&vec, index, &value) != AC_VECTOR_OK) {
                        fprintf(stderr, "Vector set failed\n");
                        goto error;
                    }
                    ref.data[index] = value;
                }
                break;
            }
            case 5: {
                int actual = 0;
                if (ref.size == 0) {
                    if (ac_vector_get(&vec, 0, &actual) != AC_VECTOR_ERR_INDEX) {
                        fprintf(stderr, "Vector get should fail on empty\n");
                        goto error;
                    }
                } else {
                    size_t index = (size_t)(rand() % ref.size);
                    if (ac_vector_get(&vec, index, &actual) != AC_VECTOR_OK ||
                        actual != ref.data[index]) {
                        fprintf(stderr, "Vector get mismatch\n");
                        goto error;
                    }
                }
                break;
            }
            case 6:
                ac_vector_clear(&vec);
                ref_vector_clear(&ref);
                break;
            case 7: {
                size_t target = ref.size + (size_t)(rand() % 20);
                if (ac_vector_reserve(&vec, target) != AC_VECTOR_OK ||
                    ref_vector_reserve(&ref, target) != 0) {
                    fprintf(stderr, "Vector reserve failed\n");
                    goto error;
                }
                break;
            }
            case 8: {
                if (ac_vector_shrink_to_fit(&vec) != AC_VECTOR_OK ||
                    ref_vector_shrink_to_fit(&ref) != 0) {
                    fprintf(stderr, "Vector shrink_to_fit failed\n");
                    goto error;
                }
                break;
            }
        }

        if (ac_vector_size(&vec) != ref.size) {
            fprintf(stderr, "Vector size diverged after iteration %zu\n", iteration);
            goto error;
        }
    }

    if (verify_vector_matches(&vec, &ref) != 0) {
        goto error;
    }

    ac_vector_destroy(&vec);
    ref_vector_destroy(&ref);
    printf("Vector stress test passed (%d iterations).\n", VECTOR_ITERATIONS);
    return 0;

error:
    ac_vector_destroy(&vec);
    ref_vector_destroy(&ref);
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
    ac_queue queue;
    reference_queue ref;

    if (ac_queue_init(&queue, sizeof(int), 0) != 0) {
        fprintf(stderr, "Failed to initialise queue\n");
        return 1;
    }
    ref_queue_init(&ref);

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
                } else {
                    if (ref_queue_dequeue(&ref, &expected) != 0 ||
                        ac_queue_dequeue(&queue, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Queue dequeue mismatch\n");
                        goto queue_error;
                    }
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
                } else {
                    if (ref_queue_peek(&ref, &expected) != 0 ||
                        ac_queue_peek(&queue, &actual) != 0 ||
                        expected != actual) {
                        fprintf(stderr, "Queue peek mismatch\n");
                        goto queue_error;
                    }
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
                }
                break;
            }
        }

        if (ac_queue_size(&queue) != ref.size) {
            fprintf(stderr, "Queue size diverged after iteration %zu\n", iteration);
            goto queue_error;
        }
    }

    while (ref.size > 0) {
        int expected = 0;
        int actual = 0;
        if (ref_queue_dequeue(&ref, &expected) != 0 ||
            ac_queue_dequeue(&queue, &actual) != 0 ||
            expected != actual) {
            fprintf(stderr, "Queue final drain mismatch\n");
            goto queue_error;
        }
    }

    ac_queue_destroy(&queue);
    ref_queue_destroy(&ref);
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
    ac_stack stack;
    reference_stack ref;

    if (ac_stack_init(&stack, sizeof(int)) != AC_VECTOR_OK) {
        fprintf(stderr, "Failed to initialise stack\n");
        return 1;
    }
    ref_stack_init(&ref);

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
                } else {
                    if (ref_stack_pop(&ref, &expected) != 0 ||
                        ac_stack_pop(&stack, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Stack pop mismatch\n");
                        goto stack_error;
                    }
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
                } else {
                    if (ref_stack_top(&ref, &expected) != 0 ||
                        ac_stack_top(&stack, &actual) != AC_VECTOR_OK ||
                        expected != actual) {
                        fprintf(stderr, "Stack top mismatch\n");
                        goto stack_error;
                    }
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
                }
                break;
        }

        if (ac_stack_size(&stack) != ref.size) {
            fprintf(stderr, "Stack size diverged after iteration %zu\n", iteration);
            goto stack_error;
        }
    }

    while (ref.size > 0) {
        int expected = 0;
        int actual = 0;
        if (ref_stack_pop(&ref, &expected) != 0 ||
            ac_stack_pop(&stack, &actual) != AC_VECTOR_OK ||
            expected != actual) {
            fprintf(stderr, "Stack final drain mismatch\n");
            goto stack_error;
        }
    }

    ac_stack_destroy(&stack);
    ref_stack_destroy(&ref);
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
    if (run_queue_stress() != 0) {
        return 1;
    }
    if (run_stack_stress() != 0) {
        return 1;
    }

    printf("All structure stress tests completed successfully.\n");
    return 0;
}
