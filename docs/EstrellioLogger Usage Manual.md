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

## Native Headers

Native interfaces are available in `src/c` and `src/cpp`.

- `src/c/estrellio_logging_lib.h` exposes the C API and compatibility macros.
- `src/c/estrellio_logging_py.h` exposes Python-style C aliases without requiring `Python.h`.
- `src/cpp/estrellio_logging_lib.h` exposes the `EstrellioLogger` C++ wrapper.

The native APIs use Python-style level values: `DEBUG=10`, `INFO=20`,
`WARNING=30`, `ERROR=40`, and `CRITICAL/FATAL=50`. Their default behavior
matches the Python helper's core semantics: threshold `WARNING`, stream output
enabled, file output disabled until a path is configured, and repeated
initialization replaces the managed file handle.
