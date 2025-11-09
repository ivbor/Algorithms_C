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

void register_vector_tests(void) {
    run_test(test_vector_push_and_get);
    run_test(test_vector_insert_and_erase);
    run_test(test_vector_pop_back);
}
