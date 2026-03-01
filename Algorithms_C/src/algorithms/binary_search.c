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
    if (data == NULL || target == NULL || compare == NULL || element_size == 0) {
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
    if (data == NULL || target == NULL || compare == NULL || element_size == 0) {
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
    if (data == NULL || target == NULL || compare == NULL || element_size == 0) {
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

ptrdiff_t ac_interpolation_search_int(const int *data, size_t size, int target) {
    if (data == NULL || size == 0) {
        return -1;
    }

    size_t low = 0;
    size_t high = size - 1;

    while (low <= high && target >= data[low] && target <= data[high]) {
        if (data[high] == data[low]) {
            return data[low] == target ? (ptrdiff_t)low : -1;
        }

        /*
         * Estimate likely position assuming quasi-uniform distribution:
         * pos = low + (target - data[low]) * (high - low) / (data[high] - data[low])
         */
        size_t pos = low +
            (size_t)(((long long)(target - data[low]) * (long long)(high - low)) /
                     (long long)(data[high] - data[low]));

        if (data[pos] == target) {
            return (ptrdiff_t)pos;
        }

        if (data[pos] < target) {
            low = pos + 1;
        } else {
            if (pos == 0) {
                break;
            }
            high = pos - 1;
        }
    }

    return -1;
}
