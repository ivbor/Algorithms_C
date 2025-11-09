#ifndef ALGORITHMS_C_UTILS_LOGGER_H
#define ALGORITHMS_C_UTILS_LOGGER_H

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

extern log_level_t current_log_level;

void rotate_logs(void);
void check_log_rotation(void);
void log_message(log_level_t level, const char *message);

#endif
