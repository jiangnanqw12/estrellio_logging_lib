#ifndef ESTRELLIO_LOGGING_LIB_H
#define ESTRELLIO_LOGGING_LIB_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESTRELLIO_LOG_LEVEL_DEBUG 10
#define ESTRELLIO_LOG_LEVEL_INFO 20
#define ESTRELLIO_LOG_LEVEL_WARNING 30
#define ESTRELLIO_LOG_LEVEL_ERROR 40
#define ESTRELLIO_LOG_LEVEL_FATAL 50

#define LOG_LEVEL_DEBUG ESTRELLIO_LOG_LEVEL_DEBUG
#define LOG_LEVEL_INFO ESTRELLIO_LOG_LEVEL_INFO
#define LOG_LEVEL_WARNING ESTRELLIO_LOG_LEVEL_WARNING
#define LOG_LEVEL_ERROR ESTRELLIO_LOG_LEVEL_ERROR
#define LOG_LEVEL_FATAL ESTRELLIO_LOG_LEVEL_FATAL

#ifndef CURRENT_LOG_LEVEL
#define CURRENT_LOG_LEVEL ESTRELLIO_LOG_LEVEL_WARNING
#endif

typedef enum {
    TIMESTAMP_MODE_OUTPUT = 1,
    TIMESTAMP_MODE_FILENAME = 2
} TimestampMode;

typedef struct EstrellioLoggingConfig {
    int level;
    int file_level;
    int stream_level;
    int log_to_file;
    int stream;
    int print_timestamp;
    const char *log_file_path;
    const char *name;
} EstrellioLoggingConfig;

extern EstrellioLoggingConfig estrellio_logging_config;

EstrellioLoggingConfig estrellio_default_config(void);
int estrellio_normalize_level_name(const char *level_name, int *level_out);
const char *estrellio_get_log_level_string(int level);
void estrellio_get_current_timestamp(char *buffer, size_t buffer_size, TimestampMode mode);

int estrellio_init_logger(const EstrellioLoggingConfig *config);
void estrellio_close_logger(void);
void estrellio_log(int level, const char *format, ...);
void estrellio_vlog(int level, const char *format, va_list args);
void estrellio_log_simple(int level, const char *format, ...);
void estrellio_vlog_simple(int level, const char *format, va_list args);
void estrellio_log_print_bytes(const uint8_t *data, size_t length);
void estrellio_log_print_binary(uint8_t value);

void log_to_console_disable(void);
void get_current_timestamp(char *buffer, size_t buffer_size, TimestampMode timestamp_mode);
void log_file_init(const char *format, ...);
void log_file_close(void);
void log_simple_message_file(int level, const char *format, ...);
void log_to_file(int level, const char *format, ...);
void log_to_console(int level, const char *format, ...);
void log_simple_message(int level, const char *format, ...);
void log_print_bytes(uint8_t *data, size_t length);
void log_print_binary(uint8_t value);

#define LOG_PRINT(level, ...) \
    do { \
        if ((level) < CURRENT_LOG_LEVEL) { \
            break; \
        } \
        estrellio_log((level), __VA_ARGS__); \
    } while (0)

#define LOG_PRINTF(level, ...) \
    do { \
        if ((level) < CURRENT_LOG_LEVEL) { \
            break; \
        } \
        estrellio_log_simple((level), __VA_ARGS__); \
    } while (0)

#if CURRENT_LOG_LEVEL <= ESTRELLIO_LOG_LEVEL_DEBUG
#define LOG_DEBUG_PRINT(...) LOG_PRINT(ESTRELLIO_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_DEBUG_PRINTF(...) LOG_PRINTF(ESTRELLIO_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define LOG_DEBUG_PRINT(...)
#define LOG_DEBUG_PRINTF(...)
#endif

#if CURRENT_LOG_LEVEL <= ESTRELLIO_LOG_LEVEL_INFO
#define LOG_INFO_PRINT(...) LOG_PRINT(ESTRELLIO_LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_INFO_PRINTF(...) LOG_PRINTF(ESTRELLIO_LOG_LEVEL_INFO, __VA_ARGS__)
#else
#define LOG_INFO_PRINT(...)
#define LOG_INFO_PRINTF(...)
#endif

#if CURRENT_LOG_LEVEL <= ESTRELLIO_LOG_LEVEL_WARNING
#define LOG_WARN_PRINT(...) LOG_PRINT(ESTRELLIO_LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_WARN_PRINTF(...) LOG_PRINTF(ESTRELLIO_LOG_LEVEL_WARNING, __VA_ARGS__)
#else
#define LOG_WARN_PRINT(...)
#define LOG_WARN_PRINTF(...)
#endif

#if CURRENT_LOG_LEVEL <= ESTRELLIO_LOG_LEVEL_ERROR
#define LOG_ERROR_PRINT(...) LOG_PRINT(ESTRELLIO_LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_ERROR_PRINTF(...) LOG_PRINTF(ESTRELLIO_LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define LOG_ERROR_PRINT(...)
#define LOG_ERROR_PRINTF(...)
#endif

#if CURRENT_LOG_LEVEL <= ESTRELLIO_LOG_LEVEL_FATAL
#define LOG_FATAL_PRINT(...) LOG_PRINT(ESTRELLIO_LOG_LEVEL_FATAL, __VA_ARGS__)
#define LOG_FATAL_PRINTF(...) LOG_PRINTF(ESTRELLIO_LOG_LEVEL_FATAL, __VA_ARGS__)
#else
#define LOG_FATAL_PRINT(...)
#define LOG_FATAL_PRINTF(...)
#endif

#define LOG_ENTRY(...) LOG_DEBUG_PRINT("Entering %s", __func__)
#define LOG_EXIT(...) LOG_DEBUG_PRINT("Exiting %s", __func__)
#define LOG_PRINT_BYTES(data, length) estrellio_log_print_bytes((const uint8_t *)(data), (length))
#define LOG_PRINT_BINARY(value) estrellio_log_print_binary((uint8_t)(value))

#ifdef __cplusplus
}
#endif

#endif
