#ifndef ALGORITHMS_C_ALGORITHMS_BINARY_SEARCH_H
#define ALGORITHMS_C_ALGORITHMS_BINARY_SEARCH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*ac_compare_fn)(const void *lhs, const void *rhs);

ptrdiff_t ac_binary_search(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
);

size_t ac_lower_bound(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
);

size_t ac_upper_bound(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
);

int ac_compare_int(const void *lhs, const void *rhs);

#ifdef __cplusplus
}
#endif

#endif
