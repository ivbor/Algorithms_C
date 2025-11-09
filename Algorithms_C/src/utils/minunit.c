#include "algorithms_c/utils/minunit.h"
#include <stdlib.h>

#define MAX_TESTS 1024

const char *passed_tests[MAX_TESTS];
const char *failed_tests[MAX_TESTS];
int passed_count = 0;
int failed_count = 0;
int tests_run = 0;
const char *current_test = NULL;

void record_pass(const char *name) {
    if (passed_count < MAX_TESTS) {
        passed_tests[passed_count++] = name;
    }
}

void record_fail(const char *name) {
    if (failed_count < MAX_TESTS) {
        failed_tests[failed_count++] = name;
    }
}

void pass() {
    printf("[PASS]\n");
}

void fail(const char *message, const char *file, int line, const char *func) {
    printf("[FAIL] %s:%d in %s() - %s\n", file, line, func, message);
}

int summary() {
    printf("\n======== Test Summary ========\n");
    printf("Total tests run: %d\n", tests_run);
    printf("Passed: %d\n", passed_count);
    for (int i = 0; i < passed_count; i++) {
        printf("  [PASS] %s\n", passed_tests[i]);
    }

    printf("Failed: %d\n", failed_count);
    for (int i = 0; i < failed_count; i++) {
        printf("  [FAIL] %s\n", failed_tests[i]);
    }

    if (failed_count == 0) {
        printf("All tests passed! 🎉\n");
    } else {
        printf("Some tests failed. 💥\n");
    }
    printf("================================\n");
    if (failed_count != 0) {
        return 1;
    } else {
        return 0;
    }
}
