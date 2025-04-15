#ifndef logger
#define logger

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

extern log_level_t current_log_level;

void rotate_logs();
void check_log_rotation();
void log_message(log_level_t level, const char *message);

#endif
