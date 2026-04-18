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
