#include "algorithms_c/algorithms/binary_search.h"
#include <stddef.h>
#include <stdint.h>

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


ptrdiff_t ac_jump_search_int(const int *data, size_t size, int target) {
    if (data == NULL || size == 0) {
        return -1;
    }

    size_t step = 1;
    while (step * step < size) {
        step++;
    }

    size_t prev = 0;
    size_t curr = step;

    while (prev < size && data[(curr < size ? curr : size) - 1] < target) {
        prev = curr;
        curr += step;
        if (prev >= size) {
            return -1;
        }
    }

    size_t end = curr < size ? curr : size;
    for (size_t i = prev; i < end; ++i) {
        if (data[i] == target) {
            return (ptrdiff_t)i;
        }
    }

    return -1;
}


ptrdiff_t ac_exponential_search_int(const int *data, size_t size, int target) {
    if (data == NULL || size == 0) {
        return -1;
    }

    if (data[0] == target) {
        return 0;
    }

    size_t bound = 1;
    while (bound < size && data[bound] < target) {
        if (bound > SIZE_MAX / 2) {
            break;
        }
        bound *= 2;
    }

    size_t left = bound / 2;
    size_t right = bound < size ? bound : size - 1;

    while (left <= right) {
        size_t mid = left + ((right - left) / 2);
        if (data[mid] == target) {
            return (ptrdiff_t)mid;
        }
        if (data[mid] < target) {
            left = mid + 1;
        } else {
            if (mid == 0) {
                break;
            }
            right = mid - 1;
        }
    }

    return -1;
}


ptrdiff_t ac_fibonacci_search_int(const int *data, size_t size, int target) {
    if (data == NULL || size == 0) {
        return -1;
    }

    size_t fib_mm2 = 0; /* (m-2)'th Fibonacci number. */
    size_t fib_mm1 = 1; /* (m-1)'th Fibonacci number. */
    size_t fib_m = fib_mm1 + fib_mm2; /* m'th Fibonacci number. */

    while (fib_m < size) {
        fib_mm2 = fib_mm1;
        fib_mm1 = fib_m;
        fib_m = fib_mm1 + fib_mm2;
    }

    ptrdiff_t offset = -1;

    while (fib_m > 1) {
        size_t i;
        if ((size_t)(offset + (ptrdiff_t)fib_mm2) >= size) {
            i = size - 1;
        } else {
            i = (size_t)(offset + (ptrdiff_t)fib_mm2);
        }

        if (data[i] < target) {
            fib_m = fib_mm1;
            fib_mm1 = fib_mm2;
            fib_mm2 = fib_m - fib_mm1;
            offset = (ptrdiff_t)i;
        } else if (data[i] > target) {
            fib_m = fib_mm2;
            fib_mm1 -= fib_mm2;
            fib_mm2 = fib_m - fib_mm1;
        } else {
            return (ptrdiff_t)i;
        }
    }

    if (fib_mm1 != 0 && offset + 1 < (ptrdiff_t)size) {
        size_t i = (size_t)(offset + 1);
        if (data[i] == target) {
            return (ptrdiff_t)i;
        }
    }

    return -1;
}
