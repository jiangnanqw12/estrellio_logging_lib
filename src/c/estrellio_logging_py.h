#ifndef ESTRELLIO_LOGGING_PY_H
#define ESTRELLIO_LOGGING_PY_H

#include "estrellio_logging_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PY_LOG_DEBUG ESTRELLIO_LOG_LEVEL_DEBUG
#define PY_LOG_INFO ESTRELLIO_LOG_LEVEL_INFO
#define PY_LOG_WARNING ESTRELLIO_LOG_LEVEL_WARNING
#define PY_LOG_ERROR ESTRELLIO_LOG_LEVEL_ERROR
#define PY_LOG_CRITICAL ESTRELLIO_LOG_LEVEL_FATAL

typedef EstrellioLoggingConfig PyLoggingConfig;

static inline PyLoggingConfig py_logging_default_config(void) {
    return estrellio_default_config();
}

static inline int py_init_logger(const PyLoggingConfig *config) {
    return estrellio_init_logger(config);
}

static inline void py_close_logger(void) {
    estrellio_close_logger();
}

static inline int py_normalize_level_name(const char *level_name, int *level_out) {
    return estrellio_normalize_level_name(level_name, level_out);
}

#ifdef __cplusplus
}
#endif

#endif
