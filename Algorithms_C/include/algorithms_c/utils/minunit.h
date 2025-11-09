/*
 * Based on
 * JTN002 - MinUnit -- a minimal unit testing framework for C
 * Jera Design LLC
 * https://jera.com/techinfo/jtns/jtn002
 */

#ifndef minunit
#define minunit

#include <stdio.h>

extern const char *current_test;

void pass();
void fail(const char *message, const char *file, int line, const char *func);
void record_pass(const char *name);
void record_fail(const char *name);
int summary();

extern int tests_run;

#define run_assert(message, test)                        \
    do {                                                 \
        if (!(test)) {                                   \
            fail(message, __FILE__, __LINE__, __func__); \
            record_fail(current_test);                   \
        } else {                                         \
            pass();                                      \
            record_pass(current_test);                   \
        }                                                \
        tests_run++;                                     \
    } while (0)

#define MU_ASSERT(condition) run_assert(#condition, (condition))

#define run_test(test)                                 \
    do {                                               \
        current_test = #test;                          \
        printf("Running test: %s...\n", current_test); \
        test();                                        \
    } while (0)

#endif
