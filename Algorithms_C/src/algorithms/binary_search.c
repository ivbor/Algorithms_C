#include "algorithms_c/algorithms/binary_search.h"
#include <stddef.h>

int ac_compare_int(const void *lhs, const void *rhs) {
    const int left = *(const int *)lhs;
    const int right = *(const int *)rhs;
    if (left < right) {
        return -1;
    }
    if (left > right) {
        return 1;
    }
    return 0;
}

ptrdiff_t ac_binary_search(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
) {
    if (data == NULL || target == NULL || compare == NULL ||
        element_size == 0) {
        return -1;
    }

    size_t left = 0;
    size_t right = size;

    while (left < right) {
        size_t mid = left + ((right - left) / 2);
        const unsigned char *base =
            (const unsigned char *)data + (mid * element_size);
        int cmp = compare(base, target);
        if (cmp == 0) {
            return (ptrdiff_t)mid;
        }
        if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return -1;
}

size_t ac_lower_bound(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
) {
    size_t left = 0;
    size_t right = size;

    while (left < right) {
        size_t mid = left + ((right - left) / 2);
        const unsigned char *base =
            (const unsigned char *)data + (mid * element_size);
        if (compare(base, target) < 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

size_t ac_upper_bound(
    const void *data,
    size_t size,
    size_t element_size,
    const void *target,
    ac_compare_fn compare
) {
    size_t left = 0;
    size_t right = size;

    while (left < right) {
        size_t mid = left + ((right - left) / 2);
        const unsigned char *base =
            (const unsigned char *)data + (mid * element_size);
        if (compare(base, target) <= 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}
