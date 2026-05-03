# estrellio_logging_lib

Small logging helpers for CLI-style Python tools.

## Install

```bash
pip install -e .
```

## Usage

```python
import logging

from estrellio_logging_lib import init_logger

logger = init_logger(
    "demo.tool",
    level=logging.WARNING,
    log_file_path="logs/demo.log",
)

logger.info("hidden by default")
logger.warning("visible in stream and log file")
```

## Behavior

- Default threshold is `WARNING`
- Stream and file handlers use the same threshold by default
- File logging is optional via `log_file_path=None` or `log_to_file=False`
- Repeated initialization does not duplicate handlers managed by the library

## Native C/C++ Interfaces

The repository also ships standalone C and C++ headers under `src/c` and
`src/cpp`. They follow the Python helper's core behavior:

- Default threshold is `WARNING`
- Stream logging is enabled by default
- File logging is enabled only when a log file path is provided
- Repeated initialization closes the previous managed file handle before opening
  the next one

Minimal C example:

```c
#include "estrellio_logging_lib.h"

int main(void) {
    EstrellioLoggingConfig config = estrellio_default_config();
    config.log_to_file = 1;
    config.log_file_path = "logs/demo.log";

    estrellio_init_logger(&config);
    LOG_INFO_PRINT("hidden by default");
    LOG_WARN_PRINT("visible warning");
    estrellio_close_logger();
    return 0;
}
```

Minimal C++ example:

```cpp
#include "estrellio_logging_lib.h"

int main() {
    EstrellioLogger::getInstance().init_logger(
        "demo.tool",
        ESTRELLIO_LOG_LEVEL_WARNING,
        "logs/demo.log",
        true);

    EstrellioLogger::getInstance().log(ESTRELLIO_LOG_LEVEL_WARNING, "visible warning");
    EstrellioLogger::getInstance().log_file_close();
    return 0;
}
```
