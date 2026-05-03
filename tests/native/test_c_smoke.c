#include "estrellio_logging_lib.h"

#include <stdio.h>

int main(int argc, char **argv) {
    EstrellioLoggingConfig config;

    if (argc != 2) {
        return 2;
    }

    config = estrellio_default_config();
    config.name = "native.c";
    config.stream = 0;
    config.log_to_file = 1;
    config.log_file_path = argv[1];

    if (estrellio_init_logger(&config) != 0) {
        return 3;
    }

    estrellio_log(ESTRELLIO_LOG_LEVEL_INFO, "hidden info");
    estrellio_log(ESTRELLIO_LOG_LEVEL_WARNING, "visible warning");
    estrellio_log(ESTRELLIO_LOG_LEVEL_ERROR, "visible error");
    estrellio_close_logger();

    return 0;
}
