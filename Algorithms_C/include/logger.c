#include "logger.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define LOG_FILE "./logs/logfile.log"
#define BACKUP_LOG_FILE "./logs/logfile_backup.log"
#define MAX_LOG_SIZE 10485760 / 1024  // 10 KB in bytes

const char *log_level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

log_level_t current_log_level = LOG_DEBUG;
FILE *log_fp = NULL;

int set_log_level(log_level_t level) {
    current_log_level = level;
    if (current_log_level == level) {
        return 0;
    } else {
        return (int)current_log_level;
    }
}

void no_log_file() {
    fprintf(stderr, "Failed to open log file: %s\n", LOG_FILE);
    exit(EXIT_FAILURE);
}

void rotate_logs() {
    if (log_fp) {
        fclose(log_fp);
    }
    remove(BACKUP_LOG_FILE);
    rename(LOG_FILE, BACKUP_LOG_FILE);
    log_fp = fopen(LOG_FILE, "w");
    if (!log_fp)
        no_log_file();
}

void check_log_rotation() {
    fseek(log_fp, 0, SEEK_END);
    long size = ftell(log_fp);
    if (size >= MAX_LOG_SIZE) {
        rotate_logs();
    }
}

void log_message(log_level_t level, const char *message) {
    if (level < current_log_level) {
        return;
    }

    log_fp = fopen(LOG_FILE, "a");

    if (!log_fp) {
        mkdir("./logs", 0700);
        log_fp = fopen(LOG_FILE, "a");
        if (!log_fp) {
            no_log_file();
            return;
        }
    }

    check_log_rotation();

    time_t now;
    time(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(log_fp, "[%s] [%s] \n", time_str, log_level_strings[level]);
    fprintf(log_fp, "%s\n", message);

    fclose(log_fp);
}
