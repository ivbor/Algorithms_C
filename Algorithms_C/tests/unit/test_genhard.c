#include "algorithms_c/algorithms/genhard.h"
#include "algorithms_c/utils/minunit.h"

static void test_genhard_reproducible_for_same_seed(void) {
    int profits_a[6], weights_a[6], capacity_a = -1;
    int profits_b[6], weights_b[6], capacity_b = -1;

    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            6, 100, 42u, 0.5, profits_a, weights_a, &capacity_a
        ) == 0
    );
    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            6, 100, 42u, 0.5, profits_b, weights_b, &capacity_b
        ) == 0
    );

    for (size_t i = 0; i < 6; ++i) {
        MU_ASSERT(profits_a[i] == profits_b[i]);
        MU_ASSERT(weights_a[i] == weights_b[i]);
    }
    MU_ASSERT(capacity_a == capacity_b);
}

static void test_genhard_generates_positive_values(void) {
    int profits[8], weights[8], capacity = -1;

    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            8, 50, 7u, 0.4, profits, weights, &capacity
        ) == 0
    );

    int sum_weights = 0;
    for (size_t i = 0; i < 8; ++i) {
        MU_ASSERT(profits[i] >= 1);
        MU_ASSERT(profits[i] <= 50);
        MU_ASSERT(weights[i] >= 1);
        MU_ASSERT(weights[i] <= 50);
        sum_weights += weights[i];
    }

    MU_ASSERT(capacity >= 1);
    MU_ASSERT(capacity <= sum_weights);
}

static void test_genhard_weakly_correlated_reproducible(void) {
    int profits_a[10], weights_a[10], cap_a = -1;
    int profits_b[10], weights_b[10], cap_b = -1;

    MU_ASSERT(
        ac_genhard_generate_weakly_correlated(
            10, 120, 123u, 0.6, profits_a, weights_a, &cap_a
        ) == 0
    );
    MU_ASSERT(
        ac_genhard_generate_weakly_correlated(
            10, 120, 123u, 0.6, profits_b, weights_b, &cap_b
        ) == 0
    );

    for (size_t i = 0; i < 10; ++i) {
        MU_ASSERT(weights_a[i] == weights_b[i]);
        MU_ASSERT(profits_a[i] == profits_b[i]);
        MU_ASSERT(profits_a[i] >= 1);
    }
    MU_ASSERT(cap_a == cap_b);
}

static void test_genhard_weakly_correlated_near_weight(void) {
    int profits[12], weights[12], capacity = -1;

    MU_ASSERT(
        ac_genhard_generate_weakly_correlated(
            12, 100, 9u, 0.5, profits, weights, &capacity
        ) == 0
    );

    int noise = 100 / 10;
    for (size_t i = 0; i < 12; ++i) {
        MU_ASSERT(profits[i] >= 1);
        MU_ASSERT(profits[i] <= weights[i] + noise);
        MU_ASSERT(profits[i] >= weights[i] - noise || profits[i] == 1);
    }
}

static void test_genhard_strongly_correlated_profit_rule(void) {
    int profits[10], weights[10], cap = -1;
    int offset = 17;

    MU_ASSERT(
        ac_genhard_generate_strongly_correlated(
            10, 80, offset, 99u, 0.5, profits, weights, &cap
        ) == 0
    );

    int sum_weights = 0;
    for (size_t i = 0; i < 10; ++i) {
        MU_ASSERT(profits[i] == weights[i] + offset);
        MU_ASSERT(weights[i] >= 1);
        MU_ASSERT(weights[i] <= 80);
        sum_weights += weights[i];
    }

    MU_ASSERT(cap >= 1);
    MU_ASSERT(cap <= sum_weights);
}

static void test_genhard_strongly_correlated_invalid(void) {
    int profits[2], weights[2], cap = 0;

    MU_ASSERT(
        ac_genhard_generate_strongly_correlated(
            2, 10, -1, 1u, 0.5, profits, weights, &cap
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_strongly_correlated(
            2, 0, 2, 1u, 0.5, profits, weights, &cap
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_strongly_correlated(
            2, 10, 2, 1u, 1.5, profits, weights, &cap
        ) == -1
    );
}

static void test_genhard_zero_items(void) {
    int capacity = -1;

    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            0, 10, 1u, 0.5, NULL, NULL, &capacity
        ) == 0
    );
    MU_ASSERT(capacity == 0);
}

static void test_genhard_invalid_arguments(void) {
    int profits[2], weights[2], capacity = 0;

    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            2, 0, 1u, 0.5, profits, weights, &capacity
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            2, 10, 1u, -0.1, profits, weights, &capacity
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            2, 10, 1u, 1.1, profits, weights, &capacity
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            2, 10, 1u, 0.5, NULL, weights, &capacity
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            2, 10, 1u, 0.5, profits, NULL, &capacity
        ) == -1
    );
    MU_ASSERT(
        ac_genhard_generate_uncorrelated(
            2, 10, 1u, 0.5, profits, weights, NULL
        ) == -1
    );
}

int main(void) {
    run_test(test_genhard_reproducible_for_same_seed);
    run_test(test_genhard_generates_positive_values);
    run_test(test_genhard_weakly_correlated_reproducible);
    run_test(test_genhard_weakly_correlated_near_weight);
    run_test(test_genhard_strongly_correlated_profit_rule);
    run_test(test_genhard_strongly_correlated_invalid);
    run_test(test_genhard_zero_items);
    run_test(test_genhard_invalid_arguments);
    return summary();
}
