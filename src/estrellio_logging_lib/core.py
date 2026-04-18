"""Logging helpers shared by CLI-style tools."""

from __future__ import annotations

import logging
import sys
from pathlib import Path
from typing import TextIO

DEFAULT_FORMAT = "%(asctime)s %(levelname)s %(name)s %(message)s"
_MANAGED_HANDLER_FLAG = "_estrellio_managed"


def normalize_level(level: int | str) -> int:
    """Return a concrete logging level from an int or logging level name."""

    if isinstance(level, int):
        return level

    if isinstance(level, str):
        normalized = level.strip().upper()
        if not normalized:
            raise ValueError("Logging level must not be empty.")
        resolved = logging.getLevelName(normalized)
        if isinstance(resolved, int):
            return resolved

    raise ValueError(f"Unsupported logging level: {level!r}")


def _coerce_formatter(formatter: logging.Formatter | str | None) -> logging.Formatter:
    if formatter is None:
        return logging.Formatter(DEFAULT_FORMAT)
    if isinstance(formatter, logging.Formatter):
        return formatter
    return logging.Formatter(formatter)


def _resolve_handler_level(
    default_level: int,
    override_level: int | str | None,
) -> int:
    if override_level is None:
        return default_level
    return normalize_level(override_level)


def _clear_managed_handlers(logger: logging.Logger) -> None:
    for handler in list(logger.handlers):
        if getattr(handler, _MANAGED_HANDLER_FLAG, False):
            logger.removeHandler(handler)
            handler.close()


def _mark_managed(handler: logging.Handler) -> logging.Handler:
    setattr(handler, _MANAGED_HANDLER_FLAG, True)
    return handler


def init_logger(
    name: str,
    *,
    level: int | str = logging.WARNING,
    file_level: int | str | None = None,
    stream_level: int | str | None = None,
    log_file_path: str | Path | None = None,
    log_to_file: bool = True,
    stream: bool = True,
    formatter: logging.Formatter | str | None = None,
    stream_target: TextIO | None = None,
) -> logging.Logger:
    """Initialize a logger with optional stream and file handlers.

    Repeated calls are idempotent for handlers managed by this library.
    """

    resolved_level = normalize_level(level)
    resolved_formatter = _coerce_formatter(formatter)
    resolved_stream_level = _resolve_handler_level(resolved_level, stream_level)
    resolved_file_level = _resolve_handler_level(resolved_level, file_level)
    active_levels: list[int] = []
    if stream:
        active_levels.append(resolved_stream_level)
    if log_to_file and log_file_path is not None:
        active_levels.append(resolved_file_level)

    logger = logging.getLogger(name)
    logger.setLevel(min(active_levels, default=resolved_level))
    logger.propagate = False
    _clear_managed_handlers(logger)

    if stream:
        stream_handler = _mark_managed(logging.StreamHandler(stream_target or sys.stderr))
        stream_handler.setLevel(resolved_stream_level)
        stream_handler.setFormatter(resolved_formatter)
        logger.addHandler(stream_handler)

    if log_to_file and log_file_path is not None:
        file_path = Path(log_file_path).expanduser()
        file_path.parent.mkdir(parents=True, exist_ok=True)
        file_handler = _mark_managed(
            logging.FileHandler(file_path, encoding="utf-8")
        )
        file_handler.setLevel(resolved_file_level)
        file_handler.setFormatter(resolved_formatter)
        logger.addHandler(file_handler)

    return logger
