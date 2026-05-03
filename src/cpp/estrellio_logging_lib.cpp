#include "estrellio_logging_lib.h"

#include <cstdio>

EstrellioLogger &EstrellioLogger::getInstance() {
    static EstrellioLogger instance;
    return instance;
}

EstrellioLogger::EstrellioLogger()
    : config(estrellio_default_config()),
      log_file_path_(),
      name_("estrellio") {
    refresh_config_view();
}

EstrellioLogger::~EstrellioLogger() {
    estrellio_close_logger();
}

void EstrellioLogger::refresh_config_view() {
    config = estrellio_logging_config;
}

int EstrellioLogger::init(const EstrellioLoggingConfig &logger_config) {
    int result;
    name_ = logger_config.name == NULL ? "estrellio" : logger_config.name;
    log_file_path_ = logger_config.log_file_path == NULL ? "" : logger_config.log_file_path;

    EstrellioLoggingConfig resolved = logger_config;
    resolved.name = name_.c_str();
    resolved.log_file_path = log_file_path_.empty() ? NULL : log_file_path_.c_str();

    result = estrellio_init_logger(&resolved);
    refresh_config_view();
    return result;
}

int EstrellioLogger::init_logger(
    const std::string &name,
    int level,
    const std::string &log_file_path,
    bool log_to_file,
    bool stream,
    int file_level,
    int stream_level,
    bool print_timestamp) {
    EstrellioLoggingConfig resolved = estrellio_default_config();
    name_ = name;
    log_file_path_ = log_file_path;

    resolved.name = name_.c_str();
    resolved.level = level;
    resolved.file_level = file_level == 0 ? level : file_level;
    resolved.stream_level = stream_level == 0 ? level : stream_level;
    resolved.log_file_path = log_file_path_.empty() ? NULL : log_file_path_.c_str();
    resolved.log_to_file = log_to_file && !log_file_path_.empty();
    resolved.stream = stream;
    resolved.print_timestamp = print_timestamp;

    return init(resolved);
}

void EstrellioLogger::init_config(bool print_timestamp, bool log_to_file, bool log_to_console) {
    EstrellioLoggingConfig resolved = estrellio_logging_config;
    resolved.print_timestamp = print_timestamp ? 1 : 0;
    resolved.log_to_file = log_to_file ? 1 : 0;
    resolved.stream = log_to_console ? 1 : 0;
    resolved.log_file_path = log_file_path_.empty() ? NULL : log_file_path_.c_str();
    resolved.name = name_.c_str();
    (void)init(resolved);
}

void EstrellioLogger::log_to_console_disable() {
    estrellio_logging_config.stream = 0;
    refresh_config_view();
}

void EstrellioLogger::log_file_init(const char *format, ...) {
    char path[4096];
    va_list args;
    EstrellioLoggingConfig resolved = estrellio_logging_config;

    va_start(args, format);
    vsnprintf(path, sizeof(path), format, args);
    va_end(args);

    log_file_path_ = path;
    resolved.log_to_file = 1;
    resolved.log_file_path = log_file_path_.c_str();
    resolved.name = name_.c_str();
    (void)init(resolved);
}

void EstrellioLogger::log_file_close() {
    estrellio_close_logger();
    refresh_config_view();
}

void EstrellioLogger::get_current_timestamp(char *buffer, size_t buffer_size, TimestampMode timestamp_mode) {
    estrellio_get_current_timestamp(buffer, buffer_size, timestamp_mode);
}

void EstrellioLogger::log_simple_message_file(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog_simple(level, format, args);
    va_end(args);
}

void EstrellioLogger::log_to_file(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog(level, format, args);
    va_end(args);
}

void EstrellioLogger::log_to_console(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog(level, format, args);
    va_end(args);
}

void EstrellioLogger::log_simple_message(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog_simple(level, format, args);
    va_end(args);
}

void EstrellioLogger::log(int level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    estrellio_vlog(level, format, args);
    va_end(args);
}

void EstrellioLogger::log_print_bytes(const uint8_t *data, size_t length) {
    estrellio_log_print_bytes(data, length);
}

void EstrellioLogger::log_print_binary(uint8_t value) {
    estrellio_log_print_binary(value);
}
