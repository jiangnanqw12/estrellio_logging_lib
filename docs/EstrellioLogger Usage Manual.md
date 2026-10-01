# estrellio_logging_lib Usage Manual

This repository maintains C/C++ logging libraries and provides a standalone
Python logging example. The existing installable Python package is retained
for compatibility with current consumers.

## Python: run or copy the example

The [standalone example](../examples/python/logging_example.py) requires only
Python 3.10+ and its standard library. From the repository root, run:

```console
python examples/python/logging_example.py
```

You can also copy that file outside this repository and run it directly. It
does not import `estrellio_logging_lib` or require an editable installation.
The demo writes DEBUG+ messages to `logs/demo.log` under the current working
directory and WARNING+ messages to stderr. Repeated runs append to the file.

For application use, copy it into your project as `logging_setup.py`, or move
its constants and initialization helpers into your project's logging module.
The demonstration `main()` can be omitted. Initialize logging in your
application entrypoint:

```python
from logging_setup import init_logger


def main():
    logger = init_logger(
        "demo.tool",
        stream_level="WARNING",
        file_level="DEBUG",
        log_file_path="logs/demo.log",
    )
    logger.debug("diagnostic detail in the file")
    logger.warning("visible in the terminal and file")


if __name__ == "__main__":
    main()
```

### Example behavior

- The default level is WARNING; `stream_level` and `file_level` override it
  independently. Levels accept integers or case-insensitive names.
- Stderr output is always enabled. Set `stream_target` to a text stream when
  needed, for example `io.StringIO()` in tests.
- File output is enabled only when `log_file_path` is provided. The helper
  creates missing parent directories and appends using UTF-8.
- The logger admits records needed by either output. Propagation to ancestor
  loggers is disabled to avoid also emitting through their handlers.
- Reinitialization removes and closes only handlers created by this helper.
  Handlers attached by the application are preserved and remain under its
  control.
- Importing the file performs no logging initialization or filesystem writes.

The example is an application configuration starting point, not a logging
framework. Adapt it locally; no synchronization with the compatibility
package is required or promised.

## Existing Python package

Existing projects may continue installing this repository with
`python -m pip install -e .` and importing:

```python
from estrellio_logging_lib import init_logger
```

The existing exports `init_logger`, `normalize_level`, and `DEFAULT_FORMAT`
remain available. The package additionally supports options such as
`log_to_file`, `stream`, and `formatter`; those options are not part of the
smaller standalone example. Existing consumers do not need to migrate as
part of this repository reorganization. For new projects, prefer the example.

## C/C++ libraries

Build and test from the repository root:

```console
cmake -S . -B build -DESTRELLIO_LOGGING_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Use `add_subdirectory(...)` from a consuming CMake project and link to
`estrellio_logging::c` or `estrellio_logging::cpp`. C++ support requires the C
implementation; both are enabled by default. See the [README](../README.md)
for minimal C and C++ examples.

- `src/c/estrellio_logging_lib.h` exposes the C API and compatibility macros.
- `src/c/estrellio_logging_py.h` exposes Python-style **C aliases** without
  requiring `Python.h`; it is not a Python binding.
- `src/cpp/estrellio_logging_lib.h` exposes the `EstrellioLogger` C++ wrapper.

The native APIs use level values DEBUG=10, INFO=20, WARNING=30, ERROR=40,
and CRITICAL/FATAL=50. Their default threshold is WARNING, stream output is
enabled, and file output is disabled until a path is configured. Repeated
initialization replaces the managed file handle. These are separate native
implementations; the Python example uses standard-library `logging` directly.
