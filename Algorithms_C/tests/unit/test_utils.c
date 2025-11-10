#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "algorithms_c/algorithms/compare.h"
#include "algorithms_c/structures/matrix_view.h"
#include "algorithms_c/utils/logger.h"
#include "algorithms_c/utils/minunit.h"

// Capture helper used to exercise the matrix printer without polluting test
// output.  We intentionally avoid fancy buffering to keep the helper
// cross-platform.
static FILE *begin_capture(int *saved_fd) {
    fflush(stdout);
    *saved_fd = dup(STDOUT_FILENO);
    FILE *capture = tmpfile();
    if (capture == NULL) {
        return NULL;
    }
    dup2(fileno(capture), STDOUT_FILENO);
    return capture;
}

static void end_capture(FILE *capture, int saved_fd) {
    fflush(stdout);
    dup2(saved_fd, STDOUT_FILENO);
    close(saved_fd);
    if (capture != NULL) {
        fclose(capture);
    }
}

static void remove_logs(void) {
    remove("./logs/logfile.log");
    remove("./logs/logfile_backup.log");
    rmdir("./logs");
}

static void test_compare_helpers(void) {
    double doubles_a[] = {1.0, 2.0, 3.0};
    double doubles_b[] = {1.0 + 1e-12, 2.0, 3.0 - 1e-12};
    MU_ASSERT(ac_compare_arrays_double(doubles_a, doubles_b, 3));

    doubles_b[1] = 4.0;
    MU_ASSERT(!ac_compare_arrays_double(doubles_a, doubles_b, 3));

    int ints_a[] = {1, 2, 3, 4};
    int ints_b[] = {1, 2, 3, 4};
    MU_ASSERT(ac_compare_arrays_int(ints_a, ints_b, 4));
    ints_b[2] = 99;
    MU_ASSERT(!ac_compare_arrays_int(ints_a, ints_b, 4));

    int matrix_a_storage[2][2] = {{1, 2}, {3, 4}};
    int matrix_b_storage[2][2] = {{1, 2}, {3, 4}};
    int *matrix_a[] = {matrix_a_storage[0], matrix_a_storage[1]};
    int *matrix_b[] = {matrix_b_storage[0], matrix_b_storage[1]};
    MU_ASSERT(ac_compare_matrices_int(matrix_a, matrix_b, 2, 2));
    matrix_b_storage[1][1] = 7;
    MU_ASSERT(!ac_compare_matrices_int(matrix_a, matrix_b, 2, 2));
}

static void test_matrix_view_prints(void) {
    int matrix_storage[2][2] = {{1, 2}, {3, 4}};
    int *matrix[] = {matrix_storage[0], matrix_storage[1]};

    int saved_fd = -1;
    FILE *capture = begin_capture(&saved_fd);
    MU_ASSERT(capture != NULL);

    ac_print_matrix(matrix, 2, 2, 0);
    ac_print_matrix(matrix, 2, 2, 1);

    fflush(stdout);
    long written = ftell(capture);
    MU_ASSERT(written > 0);
    end_capture(capture, saved_fd);
}

static void test_logger_files_and_rotation(void) {
    remove_logs();
    current_log_level = LOG_DEBUG;

    check_log_rotation();  // no-op when the logger is not initialised yet
    log_message(LOG_INFO, "initial log line");
    check_log_rotation();

    struct stat info = {0};
    MU_ASSERT(stat("./logs/logfile.log", &info) == 0);
    MU_ASSERT(info.st_size > 0);

    rotate_logs();
    struct stat backup = {0};
    MU_ASSERT(stat("./logs/logfile_backup.log", &backup) == 0);

    log_message(LOG_WARN, "post rotation");
    struct stat new_log = {0};
    MU_ASSERT(stat("./logs/logfile.log", &new_log) == 0);
    MU_ASSERT(new_log.st_size > 0);
}

int main(void) {
    run_test(test_compare_helpers);
    run_test(test_matrix_view_prints);
    run_test(test_logger_files_and_rotation);
    return summary();
}
