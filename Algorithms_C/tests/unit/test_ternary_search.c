#include <math.h>
#include "algorithms_c/algorithms/ternary_search.h"
#include "algorithms_c/utils/minunit.h"

static double parabola(double x, void *ctx) {
    double center;
    double distance;
    center = 0.0;
    if (ctx != NULL) {
        center = *(double *)ctx;
    }
    distance = x - center;
    return distance * distance;
}

static double inverted_parabola(double x, void *ctx) {
    double center;
    double distance;
    center = 0.0;
    if (ctx != NULL) {
        center = *(double *)ctx;
    }
    distance = x - center;
    return -distance * distance;
}

static void test_finds_minimum(void) {
    double expected_center;
    double result;
    int rc;
    expected_center = 3.5;
    rc = ac_ternary_search_min(
        parabola, &expected_center, 0.0, 10.0, AC_TERNARY_SEARCH_DEFAULT_EPS,
        &result
    );

    MU_ASSERT(rc == 0);
    MU_ASSERT(fabs(result - expected_center) < 1e-4);
}

static void test_finds_maximum(void) {
    double expected_center;
    double result;
    int rc;
    expected_center = -2.0;
    rc = ac_ternary_search_max(
        inverted_parabola, &expected_center, -10.0, 0.0,
        AC_TERNARY_SEARCH_DEFAULT_EPS, &result
    );

    MU_ASSERT(rc == 0);
    MU_ASSERT(fabs(result - expected_center) < 1e-4);
}

static void test_handles_reversed_interval(void) {
    double result;
    int rc;
    rc = ac_ternary_search_min(
        parabola, NULL, 5.0, -5.0, AC_TERNARY_SEARCH_DEFAULT_EPS, &result
    );

    MU_ASSERT(rc == 0);
    MU_ASSERT(fabs(result) < 1e-4);
}

static void test_rejects_bad_input(void) {
    double result;
    int rc;
    rc = ac_ternary_search_min(
        NULL, NULL, 0.0, 1.0, AC_TERNARY_SEARCH_DEFAULT_EPS, &result
    );
    MU_ASSERT(rc == -1);

    rc = ac_ternary_search_max(parabola, NULL, 0.0, 1.0, -0.5, &result);
    MU_ASSERT(rc == -1);

    rc = ac_ternary_search_max(
        parabola, NULL, 0.0, 1.0, AC_TERNARY_SEARCH_DEFAULT_EPS, NULL
    );
    MU_ASSERT(rc == -1);
}

int main(void) {
    run_test(test_finds_minimum);
    run_test(test_finds_maximum);
    run_test(test_handles_reversed_interval);
    run_test(test_rejects_bad_input);
    return summary();
}
