#ifndef ALGORITHMS_C_TESTS_STRESS_REFERENCE_DEQUE_MODEL_H
#define ALGORITHMS_C_TESTS_STRESS_REFERENCE_DEQUE_MODEL_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// reference_deque
// ----------------
// A deliberately straightforward mirror of the behaviour that the deque stress
// test expects from the production `ac_deque`. The model keeps elements in a
// densely packed `int` array so that we can manipulate the contents using
// `memmove` while also making the verification code easy to debug.

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} reference_deque;

static inline void ref_deque_init(reference_deque *ref) {
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static inline void ref_deque_destroy(reference_deque *ref) {
    free(ref->data);
    ref->data = NULL;
    ref->size = 0;
    ref->capacity = 0;
}

static inline int ref_deque_reserve(reference_deque *ref, size_t new_capacity) {
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

static inline int ref_deque_push_back(reference_deque *ref, int value) {
    if (ref->size == ref->capacity) {
        size_t new_capacity = ref->capacity == 0 ? 1 : ref->capacity * 2;
        if (ref_deque_reserve(ref, new_capacity) != 0) {
            return -1;
        }
    }

    ref->data[ref->size++] = value;
    return 0;
}

static inline int ref_deque_push_front(reference_deque *ref, int value) {
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

static inline int ref_deque_pop_front(reference_deque *ref, int *out_value) {
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

static inline int ref_deque_pop_back(reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }

    ref->size--;
    if (out_value != NULL) {
        *out_value = ref->data[ref->size];
    }
    return 0;
}

static inline int ref_deque_front(const reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[0];
    return 0;
}

static inline int ref_deque_back(const reference_deque *ref, int *out_value) {
    if (ref->size == 0) {
        return -1;
    }
    *out_value = ref->data[ref->size - 1];
    return 0;
}

static inline void ref_deque_clear(reference_deque *ref) {
    ref->size = 0;
}

#endif  // ALGORITHMS_C_TESTS_STRESS_REFERENCE_DEQUE_MODEL_H
