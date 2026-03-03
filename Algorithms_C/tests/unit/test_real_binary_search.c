#include <math.h>
#include "algorithms_c/algorithms/real_binary_search.h"
#include "algorithms_c/utils/minunit.h"

static double square_shift(double x, void *context) {
    (void)context;
    return x * x;
}

static double descending_linear(double x, void *context) {
    const double bias = *(const double *)context;
    return bias - (2.0 * x);
}

static void test_real_binary_search_ascending(void) {
    double result = 0.0;
    int status = ac_real_binary_search(
        square_shift, NULL, 9.0, 0.0, 10.0, 1e-7, true, &result
    );
    MU_ASSERT(status == 0);
    MU_ASSERT(fabs(result - 3.0) < 1e-4);
}

static void test_real_binary_search_descending(void) {
    const double bias = 10.0;
    double result = 0.0;
    int status = ac_real_binary_search(
        descending_linear, (void *)&bias, 4.0, 0.0, 10.0, 1e-7, true, &result
    );
    MU_ASSERT(status == 0);
    MU_ASSERT(fabs(result - 3.0) < 1e-4);
}

static void test_real_binary_search_unreachable_with_check(void) {
    double result = 0.0;
    int status = ac_real_binary_search(
        square_shift, NULL, -1.0, 0.0, 5.0, 1e-6, true, &result
    );
    MU_ASSERT(status == -2);
}

static void test_real_binary_search_invalid_arguments(void) {
    double result = 0.0;
    MU_ASSERT(
        ac_real_binary_search(
            NULL, NULL, 1.0, 0.0, 1.0, 1e-6, false, &result
        ) == -1
    );
    MU_ASSERT(
        ac_real_binary_search(
            square_shift, NULL, 1.0, 1.0, 0.0, 1e-6, false, &result
        ) == -1
    );
    MU_ASSERT(
        ac_real_binary_search(
            square_shift, NULL, 1.0, 0.0, 1.0, 0.0, false, &result
        ) == -1
    );
    MU_ASSERT(
        ac_real_binary_search(
            square_shift, NULL, 1.0, 0.0, 1.0, 1e-6, false, NULL
        ) == -1
    );
}

int main(void) {
    run_test(test_real_binary_search_ascending);
    run_test(test_real_binary_search_descending);
    run_test(test_real_binary_search_unreachable_with_check);
    run_test(test_real_binary_search_invalid_arguments);
    return summary();
}
