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
    if (data == NULL || target == NULL || compare == NULL ||
        element_size == 0) {
        return size;
    }

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
    if (data == NULL || target == NULL || compare == NULL ||
        element_size == 0) {
        return size;
    }

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

static int ac_bin_search_int_recursive(
    const int *data,
    size_t left,
    size_t right,
    int value
) {
    if (right <= left) {
        return 0;
    }
    if (right - left == 1U) {
        return data[left] == value ? 1 : 0;
    }

    size_t middle = left + ((right - left) / 2U);
    if (data[middle] == value) {
        return 1;
    }
    if (value < data[middle]) {
        return ac_bin_search_int_recursive(data, left, middle, value);
    }
    return ac_bin_search_int_recursive(data, middle, right, value);
}

int ac_bin_search_int(
    const int *data,
    size_t size,
    int value,
    int no_recursion
) {
    if (data == NULL || size == 0U) {
        return 0;
    }

    if (no_recursion) {
        size_t left = 0U;
        size_t right = size - 1U;

        while (left < right) {
            size_t middle = left + ((right - left) / 2U);
            if (value <= data[middle]) {
                right = middle;
            } else {
                left = middle;
            }
            if (left + 1U == right) {
                break;
            }
        }

        return (data[left] == value || data[right] == value) ? 1 : 0;
    }

    return ac_bin_search_int_recursive(data, 0U, size, value);
}
