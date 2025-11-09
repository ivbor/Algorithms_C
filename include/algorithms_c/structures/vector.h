#ifndef ALGORITHMS_C_STRUCTURES_VECTOR_H
#define ALGORITHMS_C_STRUCTURES_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AC_VECTOR_OK = 0,
    AC_VECTOR_ERR_ALLOCATION = 1,
    AC_VECTOR_ERR_INDEX = 2,
    AC_VECTOR_ERR_EMPTY = 3
} ac_vector_result;

typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} ac_vector;

int ac_vector_init(ac_vector *vec, size_t element_size);
int ac_vector_with_capacity(
    ac_vector *vec,
    size_t element_size,
    size_t capacity
);
int ac_vector_from_array(
    ac_vector *vec,
    size_t element_size,
    const void *source,
    size_t count
);

void ac_vector_destroy(ac_vector *vec);

size_t ac_vector_size(const ac_vector *vec);
size_t ac_vector_capacity(const ac_vector *vec);
bool ac_vector_empty(const ac_vector *vec);
void *ac_vector_data(ac_vector *vec);
const void *ac_vector_const_data(const ac_vector *vec);

int ac_vector_reserve(ac_vector *vec, size_t new_capacity);
int ac_vector_shrink_to_fit(ac_vector *vec);
void ac_vector_clear(ac_vector *vec);

int ac_vector_push_back(ac_vector *vec, const void *element);
int ac_vector_pop_back(ac_vector *vec, void *out_element);
int ac_vector_insert(ac_vector *vec, size_t index, const void *element);
int ac_vector_erase(ac_vector *vec, size_t index);
int ac_vector_get(const ac_vector *vec, size_t index, void *out_element);
int ac_vector_set(ac_vector *vec, size_t index, const void *element);

#ifdef __cplusplus
}
#endif

#endif
