#include "../test/test_array_count_sort.c"

#ifndef minunit
#include "../../include/minunit.h"
#endif

int main(void) {
    current_log_level = LOG_DEBUG;
    // run_test(test_array_count_sort_case_one_elt_in_2_dim_some_empty);
    run_test(test_array_count_sort_case_many_elts_no_key);
    // run_test(test_array_count_sort_case_many_elts_no_key_some_empty);
    // run_test(test_array_count_sort_case_many_elts_with_key_some_empty);
    run_test(test_array_count_sort_case_one_elt_with_huge_variation);
    run_test(test_array_count_sort_case_one_elt_in_2_dim);
    return summary();
}
