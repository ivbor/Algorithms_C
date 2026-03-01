#include "algorithms_c/algorithms/minknap.h"
#include "algorithms_c/utils/minunit.h"

static void test_minknap_basic_profit(void) {
    const int profits[] = {60, 100, 120};
    const int weights[] = {10, 20, 30};
    int selected[] = {0, 0, 0};

    int best = ac_minknap_max_profit(3, profits, weights, 50, selected);

    MU_ASSERT(best == 220);
    MU_ASSERT(selected[0] == 0);
    MU_ASSERT(selected[1] == 1);
    MU_ASSERT(selected[2] == 1);
}

static void test_minknap_zero_capacity(void) {
    const int profits[] = {5, 6};
    const int weights[] = {1, 2};
    int selected[] = {1, 1};

    int best = ac_minknap_max_profit(2, profits, weights, 0, selected);

    MU_ASSERT(best == 0);
    MU_ASSERT(selected[0] == 0);
    MU_ASSERT(selected[1] == 0);
}

static void test_minknap_invalid_inputs(void) {
    const int profits[] = {1};
    const int weights[] = {1};

    MU_ASSERT(ac_minknap_max_profit(1, NULL, weights, 1, NULL) == -1);
    MU_ASSERT(ac_minknap_max_profit(1, profits, NULL, 1, NULL) == -1);
    MU_ASSERT(ac_minknap_max_profit(1, profits, weights, -1, NULL) == -1);
}

static void test_minknap_negative_item_rejected(void) {
    const int profits[] = {5, -1};
    const int weights[] = {1, 2};

    MU_ASSERT(ac_minknap_max_profit(2, profits, weights, 3, NULL) == -1);
}

int main(void) {
    run_test(test_minknap_basic_profit);
    run_test(test_minknap_zero_capacity);
    run_test(test_minknap_invalid_inputs);
    run_test(test_minknap_negative_item_rejected);
    return summary();
}
