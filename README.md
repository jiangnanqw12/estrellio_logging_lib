# estrellio_logging_lib

Logging helpers for Python CLI tools and standalone C/C++ logging libraries.
Python 3.10+ users can install the package or copy the standard-library example.

## Python package

```console
python -m pip install estrellio-logging-lib
```

```python
from estrellio_logging_lib import init_logger

logger = init_logger("demo.tool", file_level="DEBUG", log_file_path="logs/demo.log")
logger.debug("file only")
logger.warning("terminal and file")
```

The package provides `init_logger`, `normalize_level`, and `DEFAULT_FORMAT`.
It supports separate output levels, optional file and stream handlers, and
custom formatters. Repeated initialization replaces only its own handlers.
There are no third-party runtime dependencies or native compilation steps.
For local package development, use `python -m pip install -e ".[dev]"`.

## Python example

Run from the repository root:

```console
python examples/python/logging_example.py
```

The example writes WARNING and ERROR messages to stderr, and DEBUG and higher
messages to `logs/demo.log` relative to the current working directory. File
output uses UTF-8 and appends to existing logs. Missing log directories are
created automatically.

Copy [logging_example.py](examples/python/logging_example.py) into your own
project as `logging_setup.py`, or copy its initialization helpers into your
existing logging module. Configure logging from your application's entrypoint:

```python
from logging_setup import init_logger


def main():
    logger = init_logger("demo.tool", file_level="DEBUG", log_file_path="logs/demo.log")
    logger.debug("file only")
    logger.warning("terminal and file")


if __name__ == "__main__":
    main()
```

Importing the example does not configure logging or create files. The helper
defaults to WARNING, supports separate stream/file thresholds, and replaces
only its own handlers when initialized again. Omit `log_file_path` for
terminal-only logging. Adapt the example to your project's conventions;
it is not a separately installed Python dependency.

## C/C++ build

From the repository root, with CMake 3.16+ and C/C++ compilers installed:

```console
cmake -S . -B build -DESTRELLIO_LOGGING_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

To use the libraries in another CMake project, add this repository with
`add_subdirectory(...)` and link your target to `estrellio_logging::c` or
`estrellio_logging::cpp`. The C++ target also links the C implementation.

## Native C/C++ Interfaces

The native implementations and headers live under `src/c` and `src/cpp`:

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
    config.log_file_path = "demo.log";

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
        "demo.log",
        true);

    EstrellioLogger::getInstance().log(ESTRELLIO_LOG_LEVEL_WARNING, "visible warning");
    EstrellioLogger::getInstance().log_file_close();
    return 0;
}
```

## Tests and further usage

Run Python tests with a Python environment containing pytest:

```console
python -m pytest -q tests
```

The tests cover both the existing package and the standalone example. See the
[usage manual](docs/EstrellioLogger%20Usage%20Manual.md) for integration details.

See [Publishing](docs/publishing.md) for release validation and Trusted Publishing setup.
