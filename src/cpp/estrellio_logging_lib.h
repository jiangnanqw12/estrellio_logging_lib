#ifndef ESTRELLIO_CPP_LOGGING_LIB_H
#define ESTRELLIO_CPP_LOGGING_LIB_H

#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <string>

#include "../c/estrellio_logging_lib.h"

class EstrellioLogger {
public:
    static EstrellioLogger &getInstance();

    int init(const EstrellioLoggingConfig &logger_config);
    int init_logger(
        const std::string &name = "estrellio",
        int level = ESTRELLIO_LOG_LEVEL_WARNING,
        const std::string &log_file_path = "",
        bool log_to_file = false,
        bool stream = true,
        int file_level = 0,
        int stream_level = 0,
        bool print_timestamp = true);

    void init_config(bool print_timestamp = true, bool log_to_file = true, bool log_to_console = true);
    void log_to_console_disable();
    void log_file_init(const char *format, ...);
    void log_file_close();
    void get_current_timestamp(char *buffer, size_t buffer_size, TimestampMode timestamp_mode);
    void log_simple_message_file(int level, const char *format, ...);
    void log_to_file(int level, const char *format, ...);
    void log_to_console(int level, const char *format, ...);
    void log_simple_message(int level, const char *format, ...);
    void log(int level, const char *format, ...);
    void log_print_bytes(const uint8_t *data, size_t length);
    void log_print_binary(uint8_t value);

    EstrellioLoggingConfig config;

private:
    EstrellioLogger();
    ~EstrellioLogger();
    EstrellioLogger(const EstrellioLogger &);
    EstrellioLogger &operator=(const EstrellioLogger &);

    std::string log_file_path_;
    std::string name_;

    void refresh_config_view();
};

#endif
