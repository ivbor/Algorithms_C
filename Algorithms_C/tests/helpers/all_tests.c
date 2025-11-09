#include "algorithms_c/utils/minunit.h"

void register_array_count_sort_tests(void);
void register_vector_tests(void);
void register_stack_tests(void);
void register_queue_tests(void);
void register_sorting_tests(void);
void register_binary_search_tests(void);

int main(void) {
    register_array_count_sort_tests();
    register_vector_tests();
    register_stack_tests();
    register_queue_tests();
    register_sorting_tests();
    register_binary_search_tests();

    return summary();
}
