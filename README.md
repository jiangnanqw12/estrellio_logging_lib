# estrellio_logging_lib

Standalone C/C++ logging libraries, with a copyable Python standard-library
logging example. Python 3.10+ users do not need to install this repository
to use the example.

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

## Existing Python package compatibility

The package under `src/estrellio_logging_lib`, its packaging configuration,
and its public API remain available for existing consumers such as
AutoScripts. Those consumers can continue using `python -m pip install -e .`
and `from estrellio_logging_lib import init_logger`.

New Python integrations should use the standalone example above. The example
and compatibility package have separate roles and are not guaranteed to gain
matching APIs or features. The example intentionally exposes a smaller API;
it is not a drop-in replacement for every compatibility-package option.

## Tests and further usage

Run Python tests with a Python environment containing pytest:

```console
python -m pytest -q tests
```

The tests cover both the existing package and the standalone example. See the
[usage manual](docs/EstrellioLogger%20Usage%20Manual.md) for integration details.
