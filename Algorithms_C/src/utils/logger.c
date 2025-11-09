#include "algorithms_c/utils/logger.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#undef mkdir
#define mkdir(path, mode) _mkdir(path)
#endif

#define LOG_FILE "./logs/logfile.log"
#define BACKUP_LOG_FILE "./logs/logfile_backup.log"
#define MAX_LOG_SIZE 10 * 1024 * 1024

const char *log_level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

FILE *log_fp = NULL;
log_level_t current_log_level = LOG_DEBUG;

// HELPERS

void get_timestamp(char *buffer, size_t size) {
    time_t now;
    time(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

void ensure_log_dir() {
    struct stat st = {0};
    if (stat("./logs", &st) == -1) {
        mkdir("./logs", 0700);
    }
}

void no_log_file() {
    fprintf(stderr, "Failed to open log file: %s\n", LOG_FILE);
    exit(EXIT_FAILURE);
}

// LOG ROTATION

void rotate_logs() {
    if (log_fp) {
        fclose(log_fp);
        log_fp = NULL;
    }
    remove(BACKUP_LOG_FILE);
    rename(LOG_FILE, BACKUP_LOG_FILE);
    log_fp = fopen(LOG_FILE, "w");
    if (!log_fp) {
        no_log_file();
    }
}

void check_log_rotation() {
    if (!log_fp) {
        return;
    }

    fseek(log_fp, 0, SEEK_END);
    long size = ftell(log_fp);
    if (size >= MAX_LOG_SIZE) {
        rotate_logs();
    }
    fseek(log_fp, 0, SEEK_END);
}

// ACTUAL LOGGERS

// Generic function
static void
log_message_stream(FILE *stream, log_level_t level, const char *message) {
    char time_str[20];
    get_timestamp(time_str, sizeof(time_str));
    fprintf(
        stream, "[%s] [%s] %s\n", time_str, log_level_strings[level], message
    );
    fflush(stream);
}

void log_message_file(log_level_t level, const char *message) {
    // File stream handler
    if (!log_fp) {
        ensure_log_dir();
        log_fp = fopen(LOG_FILE, "a");
        if (!log_fp) {
            no_log_file();
        }
        log_message(LOG_DEBUG, "Opened logging file successfully");
    }

    check_log_rotation();
    log_message_stream(log_fp, level, message);
}

void log_message_console(log_level_t level, const char *message) {
    log_message_stream(stdout, level, message);
}

void log_message(log_level_t level, const char *message) {
    if (level < current_log_level) {
        return;
    }
    log_message_console(level, message);
    log_message_file(level, message);
}
