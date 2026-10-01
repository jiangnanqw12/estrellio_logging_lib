"""Exercise the copyable example independently of the compatibility package."""

import io
import logging
from pathlib import Path
import runpy
import shutil
import subprocess
import sys

import pytest


EXAMPLE = Path(__file__).resolve().parents[1] / "examples/python/logging_example.py"


def test_copied_example_runs_without_site_packages(tmp_path):
    example = tmp_path / "logging_example.py"
    shutil.copyfile(EXAMPLE, example)
    result = subprocess.run(
        [sys.executable, "-I", "-S", str(example)],
        cwd=tmp_path,
        capture_output=True,
        text=True,
        check=True,
    )

    assert result.stdout == ""
    assert "debug message" not in result.stderr
    assert "warning message" in result.stderr
    assert "error message" in result.stderr
    contents = (tmp_path / "logs/demo.log").read_text(encoding="utf-8")
    assert "debug message" in contents
    assert "warning message" in contents
    assert "error message" in contents


def test_import_does_not_configure_logging_or_create_files(tmp_path):
    example = tmp_path / "logging_example.py"
    shutil.copyfile(EXAMPLE, example)
    code = """
import importlib.util
import logging
from pathlib import Path
import sys

root = logging.getLogger()
before = (root.level, root.handlers[:], dict(logging.Logger.manager.loggerDict))
spec = importlib.util.spec_from_file_location('copied_example', sys.argv[1])
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
assert before == (root.level, root.handlers[:], dict(logging.Logger.manager.loggerDict))
assert not Path('logs').exists()
assert 'estrellio_logging_lib' not in sys.modules
"""
    result = subprocess.run(
        [sys.executable, "-I", "-S", "-c", code, str(example)],
        cwd=tmp_path,
        capture_output=True,
        text=True,
        check=True,
    )
    assert result.stdout == result.stderr == ""


@pytest.mark.parametrize("verbose_output", ["file", "stream"])
def test_levels_and_reinitialization_preserve_application_handlers(tmp_path, verbose_output):
    init_logger = runpy.run_path(str(EXAMPLE))["init_logger"]
    stream = io.StringIO()
    app_stream = io.StringIO()
    log_file = tmp_path / "nested/tool.log"
    logger = logging.getLogger(f"tests.example.{verbose_output}")
    app_handler = logging.StreamHandler(app_stream)
    logger.addHandler(app_handler)
    try:
        kwargs = {
            "log_file_path": log_file,
            "stream_target": stream,
            f"{verbose_output}_level": "debug",
        }
        init_logger(logger.name, **kwargs)
        old_file_handler = next(h for h in logger.handlers if isinstance(h, logging.FileHandler))
        init_logger(logger.name, **kwargs)
        assert old_file_handler.stream is None
        assert app_handler in logger.handlers

        logger.debug("detail")
        logger.warning("visible warning 中文")
        contents = log_file.read_text(encoding="utf-8")
        assert ("detail" in contents) == (verbose_output == "file")
        assert ("detail" in stream.getvalue()) == (verbose_output == "stream")
        for output in (contents, stream.getvalue(), app_stream.getvalue()):
            assert output.count("visible warning 中文") == 1
    finally:
        for handler in list(logger.handlers):
            logger.removeHandler(handler)
            handler.close()
