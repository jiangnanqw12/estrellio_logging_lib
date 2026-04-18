from __future__ import annotations

import io
import logging

from estrellio_logging_lib import init_logger, normalize_level


def test_normalize_level_accepts_logging_name() -> None:
    assert normalize_level("warning") == logging.WARNING


def test_init_logger_filters_below_threshold(tmp_path) -> None:
    stream = io.StringIO()
    log_file = tmp_path / "logs" / "tool.log"
    logger = init_logger(
        "tests.threshold",
        level=logging.WARNING,
        log_file_path=log_file,
        stream_target=stream,
    )

    logger.info("hidden info")
    logger.warning("visible warning")

    stream_value = stream.getvalue()
    log_value = log_file.read_text(encoding="utf-8")

    assert "hidden info" not in stream_value
    assert "hidden info" not in log_value
    assert "visible warning" in stream_value
    assert "visible warning" in log_value


def test_init_logger_file_level_overrides_global_threshold(tmp_path) -> None:
    stream = io.StringIO()
    log_file = tmp_path / "logs" / "tool.log"
    logger = init_logger(
        "tests.file-override",
        level=logging.WARNING,
        file_level=logging.DEBUG,
        log_file_path=log_file,
        stream_target=stream,
    )

    logger.debug("file only debug")

    assert "file only debug" not in stream.getvalue()
    assert "file only debug" in log_file.read_text(encoding="utf-8")


def test_init_logger_stream_level_overrides_global_threshold(tmp_path) -> None:
    stream = io.StringIO()
    log_file = tmp_path / "logs" / "tool.log"
    logger = init_logger(
        "tests.stream-override",
        level=logging.WARNING,
        stream_level=logging.DEBUG,
        log_file_path=log_file,
        stream_target=stream,
    )

    logger.debug("stream only debug")

    assert "stream only debug" in stream.getvalue()
    assert "stream only debug" not in log_file.read_text(encoding="utf-8")


def test_init_logger_is_idempotent_for_managed_handlers(tmp_path) -> None:
    stream = io.StringIO()
    log_file = tmp_path / "tool.log"

    logger = init_logger(
        "tests.idempotent",
        level=logging.WARNING,
        log_file_path=log_file,
        stream_target=stream,
    )
    logger = init_logger(
        "tests.idempotent",
        level=logging.WARNING,
        log_file_path=log_file,
        stream_target=stream,
    )

    logger.error("only once")

    assert stream.getvalue().count("only once") == 1
    assert log_file.read_text(encoding="utf-8").count("only once") == 1


def test_init_logger_can_skip_file_logging() -> None:
    stream = io.StringIO()
    logger = init_logger(
        "tests.stream-only",
        level=logging.WARNING,
        log_file_path=None,
        stream_target=stream,
    )

    logger.error("stream only")

    assert "stream only" in stream.getvalue()
