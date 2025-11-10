#include "algorithms_c/structures/deque.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * The deque implementation mirrors the semantics of the Python ``Deque``
 * class but embraces a ring-buffer storage layout so that push and pop
 * operations stay amortised ``O(1)``.  The implementation code purposefully
 * documents every decision point; the comments are intentionally verbose so
 * that readers can reason about the invariants without needing to cross-reference
 * the header file.
 */

/** Allocate a raw buffer while guarding against overflow. */
static void *allocate_buffer(size_t count, size_t element_size) {
    if (count == 0 || element_size == 0) {
        return NULL;
    }
    if (count > SIZE_MAX / element_size) {
        errno = ENOMEM;
        return NULL;
    }
    return malloc(count * element_size);
}

/**
 * Resize helper that ensures ``deque`` has enough capacity for
 * ``min_capacity`` elements.  The growth policy doubles the allocation which
 * keeps amortised operations constant.
 */
static int ensure_capacity(ac_deque *deque, size_t min_capacity) {
    if (deque->capacity >= min_capacity) {
        return 0;
    }

    size_t new_capacity = deque->capacity == 0 ? 1 : deque->capacity;
    while (new_capacity < min_capacity) {
        if (new_capacity > SIZE_MAX / 2) {
            return -ENOMEM;
        }
        new_capacity *= 2;
    }

    return ac_deque_reserve(deque, new_capacity);
}

int ac_deque_init(ac_deque *deque, size_t element_size, size_t initial_capacity) {
    if (deque == NULL || element_size == 0) {
        return -EINVAL;
    }

    deque->data = allocate_buffer(initial_capacity, element_size);
    if (initial_capacity != 0 && deque->data == NULL) {
        return -ENOMEM;
    }

    deque->element_size = element_size;
    deque->capacity = initial_capacity;
    deque->size = 0;
    deque->head = 0;
    deque->tail = 0;
    return 0;
}

void ac_deque_destroy(ac_deque *deque) {
    if (deque == NULL) {
        return;
    }

    free(deque->data);
    deque->data = NULL;
    deque->element_size = 0;
    deque->capacity = 0;
    deque->size = 0;
    deque->head = 0;
    deque->tail = 0;
}

bool ac_deque_empty(const ac_deque *deque) {
    return deque == NULL || deque->size == 0;
}

size_t ac_deque_size(const ac_deque *deque) {
    return deque == NULL ? 0U : deque->size;
}

size_t ac_deque_capacity(const ac_deque *deque) {
    return deque == NULL ? 0U : deque->capacity;
}

int ac_deque_reserve(ac_deque *deque, size_t new_capacity) {
    if (deque == NULL) {
        return -EINVAL;
    }

    if (new_capacity <= deque->capacity) {
        return 0;
    }

    void *new_data = allocate_buffer(new_capacity, deque->element_size);
    if (new_data == NULL) {
        return -ENOMEM;
    }

    if (deque->size > 0 && deque->data != NULL) {
        unsigned char *dst = (unsigned char *)new_data;
        unsigned char *src = (unsigned char *)deque->data;
        size_t old_capacity = deque->capacity == 0 ? 1 : deque->capacity;
        for (size_t i = 0; i < deque->size; ++i) {
            size_t index = (deque->head + i) % old_capacity;
            memcpy(
                dst + (i * deque->element_size),
                src + (index * deque->element_size),
                deque->element_size
            );
        }
    }

    free(deque->data);
    deque->data = new_data;
    deque->capacity = new_capacity;
    deque->head = 0;
    deque->tail = deque->size;
    return 0;
}

int ac_deque_push_back(ac_deque *deque, const void *value) {
    if (deque == NULL || value == NULL) {
        return -EINVAL;
    }

    int status = ensure_capacity(deque, deque->size + 1);
    if (status != 0) {
        return status;
    }

    if (deque->capacity == 0) {
        return -ENOMEM;
    }

    unsigned char *base = (unsigned char *)deque->data;
    memcpy(base + (deque->tail * deque->element_size), value, deque->element_size);
    deque->tail = (deque->tail + 1) % deque->capacity;
    deque->size++;
    return 0;
}

int ac_deque_push_front(ac_deque *deque, const void *value) {
    if (deque == NULL || value == NULL) {
        return -EINVAL;
    }

    int status = ensure_capacity(deque, deque->size + 1);
    if (status != 0) {
        return status;
    }

    if (deque->capacity == 0) {
        return -ENOMEM;
    }

    deque->head = (deque->head == 0) ? deque->capacity - 1 : deque->head - 1;
    unsigned char *base = (unsigned char *)deque->data;
    memcpy(base + (deque->head * deque->element_size), value, deque->element_size);
    deque->size++;
    return 0;
}

int ac_deque_pop_front(ac_deque *deque, void *out_value) {
    if (deque == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0) {
        return -ENOENT;
    }

    if (out_value != NULL) {
        unsigned char *base = (unsigned char *)deque->data;
        memcpy(out_value, base + (deque->head * deque->element_size), deque->element_size);
    }

    deque->head = (deque->head + 1) % deque->capacity;
    deque->size--;
    if (deque->size == 0) {
        deque->head = 0;
        deque->tail = 0;
    }
    return 0;
}

int ac_deque_pop_back(ac_deque *deque, void *out_value) {
    if (deque == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0) {
        return -ENOENT;
    }

    deque->tail = (deque->tail == 0) ? deque->capacity - 1 : deque->tail - 1;
    if (out_value != NULL) {
        unsigned char *base = (unsigned char *)deque->data;
        memcpy(out_value, base + (deque->tail * deque->element_size), deque->element_size);
    }

    deque->size--;
    if (deque->size == 0) {
        deque->head = 0;
        deque->tail = 0;
    }
    return 0;
}

int ac_deque_front(const ac_deque *deque, void *out_value) {
    if (deque == NULL || out_value == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0) {
        return -ENOENT;
    }

    const unsigned char *base = (const unsigned char *)deque->data;
    memcpy(out_value, base + (deque->head * deque->element_size), deque->element_size);
    return 0;
}

int ac_deque_back(const ac_deque *deque, void *out_value) {
    if (deque == NULL || out_value == NULL) {
        return -EINVAL;
    }
    if (deque->size == 0) {
        return -ENOENT;
    }

    size_t index = (deque->tail == 0) ? deque->capacity - 1 : deque->tail - 1;
    const unsigned char *base = (const unsigned char *)deque->data;
    memcpy(out_value, base + (index * deque->element_size), deque->element_size);
    return 0;
}

void ac_deque_clear(ac_deque *deque) {
    if (deque == NULL) {
        return;
    }

    deque->size = 0;
    deque->head = 0;
    deque->tail = 0;
}
