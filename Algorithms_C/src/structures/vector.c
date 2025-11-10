#include "algorithms_c/structures/vector.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * This translation of the Python ``Vector`` class keeps the implementation
 * deliberately transparent: helper routines and public functions are annotated
 * with narrative comments so that the control flow mirrors the heavily
 * documented docstrings from the source material.  Even seemingly trivial
 * helpers such as ``ensure_capacity`` are explained to preserve the educational
 * tone where prose outweighs code.
 */

static int ensure_capacity(ac_vector *vec, size_t min_capacity) {
    if (vec->capacity >= min_capacity) {
        return AC_VECTOR_OK;
    }

    size_t new_capacity = vec->capacity == 0 ? 1 : vec->capacity;
    while (new_capacity < min_capacity) {
        if (new_capacity > SIZE_MAX / 2) {
            return AC_VECTOR_ERR_ALLOCATION;
        }
        new_capacity *= 2;
    }

    return ac_vector_reserve(vec, new_capacity);
}

/** Allocate zero-initialised storage while defending against overflow. */
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

int ac_vector_init(ac_vector *vec, size_t element_size) {
    if (vec == NULL || element_size == 0) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->element_size = element_size;
    return AC_VECTOR_OK;
}

int ac_vector_with_capacity(
    ac_vector *vec,
    size_t element_size,
    size_t capacity
) {
    if (vec == NULL || element_size == 0) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->data = allocate_buffer(capacity, element_size);
    if (capacity != 0 && vec->data == NULL) {
        vec->size = 0;
        vec->capacity = 0;
        vec->element_size = element_size;
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->size = 0;
    vec->capacity = capacity;
    vec->element_size = element_size;
    return AC_VECTOR_OK;
}

int ac_vector_from_array(
    ac_vector *vec,
    size_t element_size,
    const void *source,
    size_t count
) {
    if (vec == NULL || element_size == 0) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->data = allocate_buffer(count, element_size);
    if (count != 0 && vec->data == NULL) {
        vec->size = 0;
        vec->capacity = 0;
        vec->element_size = element_size;
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->element_size = element_size;
    vec->capacity = count;
    vec->size = count;
    if (count != 0 && source != NULL) {
        memcpy(vec->data, source, count * element_size);
    }

    return AC_VECTOR_OK;
}

void ac_vector_destroy(ac_vector *vec) {
    if (vec == NULL) {
        return;
    }

    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    vec->element_size = 0;
}

size_t ac_vector_size(const ac_vector *vec) {
    return vec != NULL ? vec->size : 0U;
}

size_t ac_vector_capacity(const ac_vector *vec) {
    return vec != NULL ? vec->capacity : 0U;
}

bool ac_vector_empty(const ac_vector *vec) {
    return ac_vector_size(vec) == 0U;
}

void *ac_vector_data(ac_vector *vec) {
    return vec != NULL ? vec->data : NULL;
}

const void *ac_vector_const_data(const ac_vector *vec) {
    return vec != NULL ? vec->data : NULL;
}

int ac_vector_reserve(ac_vector *vec, size_t new_capacity) {
    if (vec == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (new_capacity <= vec->capacity) {
        return AC_VECTOR_OK;
    }

    void *new_data = realloc(vec->data, new_capacity * vec->element_size);
    if (new_data == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->data = new_data;
    vec->capacity = new_capacity;
    return AC_VECTOR_OK;
}

int ac_vector_shrink_to_fit(ac_vector *vec) {
    if (vec == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (vec->size == vec->capacity) {
        return AC_VECTOR_OK;
    }

    if (vec->size == 0) {
        free(vec->data);
        vec->data = NULL;
        vec->capacity = 0;
        return AC_VECTOR_OK;
    }

    void *new_data = realloc(vec->data, vec->size * vec->element_size);
    if (new_data == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    vec->data = new_data;
    vec->capacity = vec->size;
    return AC_VECTOR_OK;
}

void ac_vector_clear(ac_vector *vec) {
    if (vec == NULL) {
        return;
    }

    vec->size = 0;
}

int ac_vector_push_back(ac_vector *vec, const void *element) {
    if (vec == NULL || element == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    int status = ensure_capacity(vec, vec->size + 1);
    if (status != AC_VECTOR_OK) {
        return status;
    }

    unsigned char *target =
        (unsigned char *)vec->data + (vec->size * vec->element_size);
    memcpy(target, element, vec->element_size);
    vec->size++;
    return AC_VECTOR_OK;
}

int ac_vector_pop_back(ac_vector *vec, void *out_element) {
    if (vec == NULL || vec->size == 0) {
        return AC_VECTOR_ERR_EMPTY;
    }

    vec->size--;
    if (out_element != NULL) {
        unsigned char *source =
            (unsigned char *)vec->data + (vec->size * vec->element_size);
        memcpy(out_element, source, vec->element_size);
    }
    return AC_VECTOR_OK;
}

int ac_vector_insert(ac_vector *vec, size_t index, const void *element) {
    if (vec == NULL || element == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (index > vec->size) {
        return AC_VECTOR_ERR_INDEX;
    }

    int status = ensure_capacity(vec, vec->size + 1);
    if (status != AC_VECTOR_OK) {
        return status;
    }

    unsigned char *base = (unsigned char *)vec->data;
    memmove(
        base + ((index + 1) * vec->element_size),
        base + (index * vec->element_size),
        (vec->size - index) * vec->element_size
    );
    memcpy(base + (index * vec->element_size), element, vec->element_size);
    vec->size++;
    return AC_VECTOR_OK;
}

int ac_vector_erase(ac_vector *vec, size_t index) {
    if (vec == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (index >= vec->size) {
        return AC_VECTOR_ERR_INDEX;
    }

    unsigned char *base = (unsigned char *)vec->data;
    memmove(
        base + (index * vec->element_size),
        base + ((index + 1) * vec->element_size),
        (vec->size - index - 1) * vec->element_size
    );
    vec->size--;
    return AC_VECTOR_OK;
}

int ac_vector_get(const ac_vector *vec, size_t index, void *out_element) {
    if (vec == NULL || out_element == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (index >= vec->size) {
        return AC_VECTOR_ERR_INDEX;
    }

    const unsigned char *source =
        (const unsigned char *)vec->data + (index * vec->element_size);
    memcpy(out_element, source, vec->element_size);
    return AC_VECTOR_OK;
}

int ac_vector_set(ac_vector *vec, size_t index, const void *element) {
    if (vec == NULL || element == NULL) {
        return AC_VECTOR_ERR_ALLOCATION;
    }

    if (index >= vec->size) {
        return AC_VECTOR_ERR_INDEX;
    }

    unsigned char *target =
        (unsigned char *)vec->data + (index * vec->element_size);
    memcpy(target, element, vec->element_size);
    return AC_VECTOR_OK;
}
