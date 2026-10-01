"""Copyable Python 3.10+ logging setup using only the standard library.

Run this file directly to write WARNING+ messages to stderr and DEBUG+
messages to logs/demo.log, relative to the current working directory.
Copy the initialization helpers into your application's logging module;
call init_logger() from its entrypoint, not during module import.
"""

from __future__ import annotations

import logging
import sys
from pathlib import Path
from typing import TextIO


DEFAULT_FORMAT = "%(asctime)s %(levelname)s %(name)s %(message)s"
_MANAGED_HANDLER_FLAG = "_logging_example_managed"


def normalize_level(level: int | str) -> int:
    """Accept integer levels and case-insensitive names such as 'warning'."""
    if isinstance(level, int):
        return level
    if isinstance(level, str):
        resolved = logging.getLevelName(level.strip().upper())
        if isinstance(resolved, int):
            return resolved
    raise ValueError(f"Unsupported logging level: {level!r}")


def init_logger(
    name: str,
    *,
    level: int | str = logging.WARNING,
    stream_level: int | str | None = None,
    file_level: int | str | None = None,
    log_file_path: str | Path | None = None,
    stream_target: TextIO | None = None,
) -> logging.Logger:
    """Configure stderr and an optional UTF-8 file with independent levels.

    Only handlers created by this helper are replaced on repeated calls.
    Existing handlers belonging to the application are left attached.
    """
    default_level = normalize_level(level)
    stream_threshold = normalize_level(stream_level) if stream_level is not None else default_level
    file_threshold = normalize_level(file_level) if file_level is not None else default_level
    formatter = logging.Formatter(DEFAULT_FORMAT)
    logger = logging.getLogger(name)
    # The logger must admit records needed by either output handler.
    logger.setLevel(min(stream_threshold, file_threshold) if log_file_path is not None else stream_threshold)
    logger.propagate = False

    for handler in list(logger.handlers):
        if getattr(handler, _MANAGED_HANDLER_FLAG, False):
            logger.removeHandler(handler)
            handler.close()

    stream_handler = logging.StreamHandler(stream_target if stream_target is not None else sys.stderr)
    stream_handler.setLevel(stream_threshold)
    stream_handler.setFormatter(formatter)
    setattr(stream_handler, _MANAGED_HANDLER_FLAG, True)
    logger.addHandler(stream_handler)

    if log_file_path is not None:
        file_path = Path(log_file_path).expanduser()
        file_path.parent.mkdir(parents=True, exist_ok=True)
        file_handler = logging.FileHandler(file_path, encoding="utf-8")
        file_handler.setLevel(file_threshold)
        file_handler.setFormatter(formatter)
        setattr(file_handler, _MANAGED_HANDLER_FLAG, True)
        logger.addHandler(file_handler)

    return logger


def main() -> None:
    logger = init_logger(
        "logging_example",
        file_level="DEBUG",
        log_file_path=Path("logs") / "demo.log",
    )
    logger.debug("debug message: file only")
    logger.warning("warning message: terminal and file")
    logger.error("error message: terminal and file")


if __name__ == "__main__":
    main()
