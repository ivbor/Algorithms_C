#include "algorithms_c/algorithms/bounds.h"

ptrdiff_t ac_bounds_lower_int(const int *array, size_t size, int value) {
    if (array == NULL || size == 0U) {
        return -1;
    }

    size_t left = 0U;
    size_t right = size;
    while (left < right) {
        size_t middle = left + ((right - left) / 2U);
        if (array[middle] < value) {
            left = middle + 1U;
        } else {
            right = middle;
        }
    }

    if (left < size && array[left] == value) {
        return (ptrdiff_t)left;
    }
    return -1;
}

ptrdiff_t ac_bounds_upper_int(const int *array, size_t size, int value) {
    if (array == NULL || size == 0U) {
        return -1;
    }

    size_t left = 0U;
    size_t right = size;
    while (left < right) {
        size_t middle = left + ((right - left) / 2U);
        if (array[middle] <= value) {
            left = middle + 1U;
        } else {
            right = middle;
        }
    }

    if (left > 0U && array[left - 1U] == value) {
        return (ptrdiff_t)(left - 1U);
    }
    return -1;
}
