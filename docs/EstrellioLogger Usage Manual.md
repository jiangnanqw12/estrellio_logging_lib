# estrellio_logging_lib Usage Manual

## Introduction

`estrellio_logging_lib` exposes one main entrypoint: `init_logger()`.
It configures a standard `logging.Logger` with a stream handler and an
optional file handler that share the same logging threshold.

## Example

```python
import logging

from estrellio_logging_lib import init_logger


def main() -> None:
    logger = init_logger(
        "demo.tool",
        level=logging.WARNING,
        log_file_path="logs/demo.log",
    )

    logger.info("hidden by default")
    logger.warning("visible warning")
    logger.error("visible error")


if __name__ == "__main__":
    main()
```

## Behavior

- Default threshold is `WARNING`
- Stream and file handlers use the same threshold
- File logging can be disabled with `log_file_path=None` or `log_to_file=False`
- Repeated initialization removes and recreates handlers managed by the library, so log lines are not duplicated
