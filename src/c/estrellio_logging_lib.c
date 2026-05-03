#include "estrellio_logging_lib.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <direct.h>
#define ESTRELLIO_MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define ESTRELLIO_MKDIR(path) mkdir((path), 0777)
#endif

static FILE *estrellio_log_file = NULL;
static char estrellio_log_file_path[4096] = "";
static char estrellio_logger_name[128] = "estrellio";

EstrellioLoggingConfig estrellio_logging_config = {
    ESTRELLIO_LOG_LEVEL_WARNING,
    ESTRELLIO_LOG_LEVEL_WARNING,
    ESTRELLIO_LOG_LEVEL_WARNING,
    0,
    1,
    1,
    NULL,
    "estrellio",
};

static int estrellio_should_log_to_stream(int level) {
    return estrellio_logging_config.stream && level >= estrellio_logging_config.stream_level;
}

static int estrellio_should_log_to_file(int level) {
    return estrellio_logging_config.log_to_file && estrellio_log_file != NULL && level >= estrellio_logging_config.file_level;
}

static int estrellio_is_path_separator(char value) {
    return value == '/' || value == '\\';
}

static int estrellio_create_parent_dirs(const char *file_path) {
    char path[4096];
    size_t length;
    size_t i;

    if (file_path == NULL || file_path[0] == '\0') {
        return 0;
    }

    length = strlen(file_path);
    if (length >= sizeof(path)) {
        return -1;
    }

    memcpy(path, file_path, length + 1);
    for (i = 1; i < length; ++i) {
        if (!estrellio_is_path_separator(path[i])) {
            continue;
        }
        path[i] = '\0';
        if (path[0] != '\0' && ESTRELLIO_MKDIR(path) != 0 && errno != EEXIST) {
            path[i] = file_path[i];
            return -1;
        }
        path[i] = file_path[i];
    }

    return 0;
}

EstrellioLoggingConfig estrellio_default_config(void) {
    EstrellioLoggingConfig config;
    config.level = ESTRELLIO_LOG_LEVEL_WARNING;
    config.file_level = ESTRELLIO_LOG_LEVEL_WARNING;
    config.stream_level = ESTRELLIO_LOG_LEVEL_WARNING;
    config.log_to_file = 0;
    config.stream = 1;
    config.print_timestamp = 1;
    config.log_file_path = NULL;
    config.name = "estrellio";
    return config;
}

int estrellio_normalize_level_name(const char *level_name, int *level_out) {
    char normalized[16];
    size_t i;

    if (level_name == NULL || level_out == NULL || level_name[0] == '\0') {
        return -1;
    }

    for (i = 0; i < sizeof(normalized) - 1 && level_name[i] != '\0'; ++i) {
        normalized[i] = (char)toupper((unsigned char)level_name[i]);
    }
    normalized[i] = '\0';

    if (strcmp(normalized, "DEBUG") == 0) {
        *level_out = ESTRELLIO_LOG_LEVEL_DEBUG;
        return 0;
    }
    if (strcmp(normalized, "INFO") == 0) {
        *level_out = ESTRELLIO_LOG_LEVEL_INFO;
        return 0;
    }
    if (strcmp(normalized, "WARNING") == 0 || strcmp(normalized, "WARN") == 0) {
        *level_out = ESTRELLIO_LOG_LEVEL_WARNING;
        return 0;
    }
    if (strcmp(normalized, "ERROR") == 0) {
        *level_out = ESTRELLIO_LOG_LEVEL_ERROR;
        return 0;
    }
    if (strcmp(normalized, "FATAL") == 0 || strcmp(normalized, "CRITICAL") == 0) {
        *level_out = ESTRELLIO_LOG_LEVEL_FATAL;
        return 0;
    }

    return -1;
}

const char *estrellio_get_log_level_string(int level) {
    switch (level) {
        case ESTRELLIO_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case ESTRELLIO_LOG_LEVEL_INFO:
            return "INFO";
        case ESTRELLIO_LOG_LEVEL_WARNING:
            return "WARNING";
        case ESTRELLIO_LOG_LEVEL_ERROR:
            return "ERROR";
        case ESTRELLIO_LOG_LEVEL_FATAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

void estrellio_get_current_timestamp(char *buffer, size_t buffer_size, TimestampMode mode) {
    time_t raw_time;
    struct tm *time_info;

    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    time(&raw_time);
    time_info = localtime(&raw_time);
    if (time_info == NULL) {
        buffer[0] = '\0';
        return;
    }

    if (mode == TIMESTAMP_MODE_OUTPUT) {
        strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", time_info);
    } else if (mode == TIMESTAMP_MODE_FILENAME) {
        strftime(buffer, buffer_size, "%Y-%m-%d %H.%M.%S", time_info);
    } else {
        buffer[0] = '\0';
    }
}

int estrellio_init_logger(const EstrellioLoggingConfig *config) {
    EstrellioLoggingConfig resolved = estrellio_default_config();
    int base_level;
    int active_level;
    size_t name_length;

    estrellio_close_logger();

    if (config != NULL) {
        resolved = *config;
        if (resolved.level <= 0) {
            resolved.level = ESTRELLIO_LOG_LEVEL_WARNING;
        }
        if (resolved.file_level <= 0) {
            resolved.file_level = resolved.level;
        }
        if (resolved.stream_level <= 0) {
            resolved.stream_level = resolved.level;
        }
        if (resolved.name == NULL) {
            resolved.name = "estrellio";
        }
    }

    base_level = resolved.level;
    active_level = base_level;
    if (resolved.stream && resolved.stream_level < active_level) {
        active_level = resolved.stream_level;
    }
    if (resolved.log_to_file && resolved.log_file_path != NULL && resolved.log_file_path[0] != '\0' &&
        resolved.file_level < active_level) {
        active_level = resolved.file_level;
    }
    resolved.level = active_level;

    estrellio_logging_config = resolved;
    name_length = strlen(resolved.name);
    if (name_length >= sizeof(estrellio_logger_name)) {
        name_length = sizeof(estrellio_logger_name) - 1;
    }
    memcpy(estrellio_logger_name, resolved.name, name_length);
    estrellio_logger_name[name_length] = '\0';
    estrellio_logging_config.name = estrellio_logger_name;

    estrellio_log_file_path[0] = '\0';
    if (resolved.log_to_file && resolved.log_file_path != NULL && resolved.log_file_path[0] != '\0') {
        size_t path_length = strlen(resolved.log_file_path);
        if (path_length >= sizeof(estrellio_log_file_path)) {
            return -1;
        }
        if (estrellio_create_parent_dirs(resolved.log_file_path) != 0) {
            return -1;
        }
        memcpy(estrellio_log_file_path, resolved.log_file_path, path_length + 1);
        estrellio_log_file = fopen(estrellio_log_file_path, "a");
        if (estrellio_log_file == NULL) {
            return -1;
        }
        estrellio_logging_config.log_file_path = estrellio_log_file_path;
    } else {
        estrellio_logging_config.log_to_file = 0;
        estrellio_logging_config.log_file_path = NULL;
    }

    return 0;
}

void estrellio_close_logger(void) {
    if (estrellio_log_file != NULL) {
        fclose(estrellio_log_file);
        estrellio_log_file = NULL;
    }
}

static void estrellio_write_prefix(FILE *target, int level) {
    if (estrellio_logging_config.print_timestamp) {
        char timestamp[32];
        estrellio_get_current_timestamp(timestamp, sizeof(timestamp), TIMESTAMP_MODE_OUTPUT);
        fprintf(target, "%s %s %s ", timestamp, estrellio_get_log_level_string(level), estrellio_logger_name);
    } else {
        fprintf(target, "%s %s ", estrellio_get_log_level_string(level), estrellio_logger_name);
    }
}

static void estrellio_vwrite(FILE *target, int level, const char *format, va_list args, int with_prefix) {
    if (with_prefix) {
        estrellio_write_prefix(target, level);
    }
    vfprintf(target, format, args);
    fputc('\n', target);
    fflush(target);
}

void estrellio_vlog(int level, const char *format, va_list args) {
    if (format == NULL || level < estrellio_logging_config.level) {
        return;
    }

    if (estrellio_should_log_to_stream(level)) {
        va_list stream_args;
        va_copy(stream_args, args);
        estrellio_vwrite(stderr, level, format, stream_args, 1);
        va_end(stream_args);
    }

    if (estrellio_should_log_to_file(level)) {
        va_list file_args;
        va_copy(file_args, args);
        estrellio_vwrite(estrellio_log_file, level, format, file_args, 1);
        va_end(file_args);
    }
}

void estrellio_log(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog(level, format, args);
    va_end(args);
}

void estrellio_vlog_simple(int level, const char *format, va_list args) {
    if (format == NULL || level < estrellio_logging_config.level) {
        return;
    }

    if (estrellio_should_log_to_stream(level)) {
        va_list stream_args;
        va_copy(stream_args, args);
        estrellio_vwrite(stderr, level, format, stream_args, 0);
        va_end(stream_args);
    }

    if (estrellio_should_log_to_file(level)) {
        va_list file_args;
        va_copy(file_args, args);
        estrellio_vwrite(estrellio_log_file, level, format, file_args, 0);
        va_end(file_args);
    }
}

void estrellio_log_simple(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog_simple(level, format, args);
    va_end(args);
}

void estrellio_log_print_bytes(const uint8_t *data, size_t length) {
    size_t i;
    if (data == NULL) {
        return;
    }
    for (i = 0; i < length; ++i) {
        estrellio_log_simple(ESTRELLIO_LOG_LEVEL_DEBUG, "0x%02X", data[i]);
    }
}

void estrellio_log_print_binary(uint8_t value) {
    char output[9];
    int i;
    for (i = 7; i >= 0; --i) {
        output[7 - i] = (value & (1u << i)) ? '1' : '0';
    }
    output[8] = '\0';
    estrellio_log_simple(ESTRELLIO_LOG_LEVEL_DEBUG, "%s", output);
}

void log_to_console_disable(void) {
    estrellio_logging_config.stream = 0;
}

void get_current_timestamp(char *buffer, size_t buffer_size, TimestampMode timestamp_mode) {
    estrellio_get_current_timestamp(buffer, buffer_size, timestamp_mode);
}

void log_file_init(const char *format, ...) {
    char path[4096];
    EstrellioLoggingConfig config = estrellio_logging_config;
    va_list args;

    va_start(args, format);
    vsnprintf(path, sizeof(path), format, args);
    va_end(args);

    config.log_to_file = 1;
    config.log_file_path = path;
    (void)estrellio_init_logger(&config);
}

void log_file_close(void) {
    estrellio_close_logger();
}

void log_simple_message_file(int level, const char *format, ...) {
    va_list args;
    if (!estrellio_should_log_to_file(level)) {
        return;
    }
    va_start(args, format);
    estrellio_vwrite(estrellio_log_file, level, format, args, 0);
    va_end(args);
}

void log_to_file(int level, const char *format, ...) {
    va_list args;
    if (!estrellio_should_log_to_file(level)) {
        return;
    }
    va_start(args, format);
    estrellio_vwrite(estrellio_log_file, level, format, args, 1);
    va_end(args);
}

void log_to_console(int level, const char *format, ...) {
    va_list args;
    if (!estrellio_should_log_to_stream(level)) {
        return;
    }
    va_start(args, format);
    estrellio_vwrite(stderr, level, format, args, 1);
    va_end(args);
}

void log_simple_message(int level, const char *format, ...) {
    va_list args;
    if (!estrellio_should_log_to_stream(level)) {
        return;
    }
    va_start(args, format);
    estrellio_vwrite(stderr, level, format, args, 0);
    va_end(args);
}

void log_print_bytes(uint8_t *data, size_t length) {
    estrellio_log_print_bytes(data, length);
}

void log_print_binary(uint8_t value) {
    estrellio_log_print_binary(value);
}
