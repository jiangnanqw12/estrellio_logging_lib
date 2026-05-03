#include "estrellio_logging_lib.h"

int main(int argc, char **argv) {
    EstrellioLogger &logger = EstrellioLogger::getInstance();

    if (argc != 2) {
        return 2;
    }

    if (logger.init_logger("native.cpp", ESTRELLIO_LOG_LEVEL_WARNING, argv[1], true, false) != 0) {
        return 3;
    }
    if (logger.init_logger("native.cpp", ESTRELLIO_LOG_LEVEL_WARNING, argv[1], true, false) != 0) {
        return 4;
    }

    logger.log(ESTRELLIO_LOG_LEVEL_INFO, "hidden info");
    logger.log(ESTRELLIO_LOG_LEVEL_WARNING, "visible warning");
    logger.log(ESTRELLIO_LOG_LEVEL_ERROR, "visible error");
    logger.log_file_close();

    return 0;
}
