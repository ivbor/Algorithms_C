#include <string.h>
#include "algorithms_c/structures/vector.h"
#include "algorithms_c/utils/minunit.h"

static void test_vector_push_and_get(void) {
    ac_vector vector;
    MU_ASSERT(ac_vector_init(&vector, sizeof(int)) == AC_VECTOR_OK);

    for (int value = 0; value < 100; ++value) {
        MU_ASSERT(ac_vector_push_back(&vector, &value) == AC_VECTOR_OK);
    }

    MU_ASSERT(ac_vector_size(&vector) == 100);
    for (size_t i = 0; i < ac_vector_size(&vector); ++i) {
        int out = 0;
        MU_ASSERT(ac_vector_get(&vector, i, &out) == AC_VECTOR_OK);
        MU_ASSERT(out == (int)i);
    }

    ac_vector_destroy(&vector);
}

static void test_vector_insert_and_erase(void) {
    ac_vector vector;
    MU_ASSERT(ac_vector_init(&vector, sizeof(int)) == AC_VECTOR_OK);

    for (int value = 0; value < 5; ++value) {
        MU_ASSERT(ac_vector_push_back(&vector, &value) == AC_VECTOR_OK);
    }

    int inserted = 42;
    MU_ASSERT(ac_vector_insert(&vector, 2, &inserted) == AC_VECTOR_OK);

    int out = 0;
    MU_ASSERT(ac_vector_get(&vector, 2, &out) == AC_VECTOR_OK);
    MU_ASSERT(out == 42);

    MU_ASSERT(ac_vector_erase(&vector, 2) == AC_VECTOR_OK);
    MU_ASSERT(ac_vector_get(&vector, 2, &out) == AC_VECTOR_OK);
    MU_ASSERT(out == 2);

    ac_vector_destroy(&vector);
}

static void test_vector_pop_back(void) {
    ac_vector vector;
    MU_ASSERT(ac_vector_init(&vector, sizeof(int)) == AC_VECTOR_OK);

    for (int value = 0; value < 3; ++value) {
        MU_ASSERT(ac_vector_push_back(&vector, &value) == AC_VECTOR_OK);
    }

    int out = 0;
    MU_ASSERT(ac_vector_pop_back(&vector, &out) == AC_VECTOR_OK);
    MU_ASSERT(out == 2);
    MU_ASSERT(ac_vector_size(&vector) == 2);

    ac_vector_destroy(&vector);
}

static void test_vector_with_capacity_and_reserve(void) {
    ac_vector vector;
    MU_ASSERT(ac_vector_with_capacity(&vector, sizeof(int), 4) == AC_VECTOR_OK);

    MU_ASSERT(ac_vector_capacity(&vector) == 4);
    MU_ASSERT(ac_vector_size(&vector) == 0);

    for (int value = 0; value < 4; ++value) {
        MU_ASSERT(ac_vector_push_back(&vector, &value) == AC_VECTOR_OK);
    }

    MU_ASSERT(ac_vector_capacity(&vector) == 4);

    MU_ASSERT(ac_vector_reserve(&vector, 16) == AC_VECTOR_OK);
    MU_ASSERT(ac_vector_capacity(&vector) >= 16);

    ac_vector_destroy(&vector);
}

static void test_vector_from_array_and_clear(void) {
    const int values[] = {10, 20, 30, 40, 50};

    ac_vector vector;
    MU_ASSERT(
        ac_vector_from_array(
            &vector, sizeof(int), values, sizeof(values) / sizeof(values[0])
        )
        == AC_VECTOR_OK
    );

    MU_ASSERT(ac_vector_size(&vector) == sizeof(values) / sizeof(values[0]));
    MU_ASSERT(ac_vector_capacity(&vector) == sizeof(values) / sizeof(values[0]));

    for (size_t i = 0; i < ac_vector_size(&vector); ++i) {
        int out = 0;
        MU_ASSERT(ac_vector_get(&vector, i, &out) == AC_VECTOR_OK);
        MU_ASSERT(out == values[i]);
    }

    ac_vector_clear(&vector);
    MU_ASSERT(ac_vector_size(&vector) == 0);
    MU_ASSERT(ac_vector_capacity(&vector) == sizeof(values) / sizeof(values[0]));

    ac_vector_destroy(&vector);
}

static void test_vector_set_shrink_and_errors(void) {
    ac_vector vector;
    MU_ASSERT(ac_vector_init(&vector, sizeof(int)) == AC_VECTOR_OK);

    for (int value = 0; value < 5; ++value) {
        MU_ASSERT(ac_vector_push_back(&vector, &value) == AC_VECTOR_OK);
    }

    int replacement = 99;
    MU_ASSERT(ac_vector_set(&vector, 1, &replacement) == AC_VECTOR_OK);

    int out = 0;
    MU_ASSERT(ac_vector_get(&vector, 1, &out) == AC_VECTOR_OK);
    MU_ASSERT(out == replacement);

    MU_ASSERT(ac_vector_pop_back(&vector, NULL) == AC_VECTOR_OK);
    MU_ASSERT(ac_vector_shrink_to_fit(&vector) == AC_VECTOR_OK);
    MU_ASSERT(ac_vector_capacity(&vector) == ac_vector_size(&vector));

    MU_ASSERT(ac_vector_get(&vector, 100, &out) == AC_VECTOR_ERR_INDEX);
    MU_ASSERT(ac_vector_pop_back(&vector, &out) == AC_VECTOR_OK);

    ac_vector_clear(&vector);
    MU_ASSERT(ac_vector_pop_back(&vector, &out) == AC_VECTOR_ERR_EMPTY);

    MU_ASSERT(ac_vector_push_back(&vector, NULL) == AC_VECTOR_ERR_ALLOCATION);

    ac_vector_destroy(&vector);
}

int main(void) {
    run_test(test_vector_push_and_get);
    run_test(test_vector_insert_and_erase);
    run_test(test_vector_pop_back);
    run_test(test_vector_with_capacity_and_reserve);
    run_test(test_vector_from_array_and_clear);
    run_test(test_vector_set_shrink_and_errors);
    return summary();
}
