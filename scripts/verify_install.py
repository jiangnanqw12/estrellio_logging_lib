"""Test the installed distribution outside the source tree, without conftest."""

from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    expected_version = sys.argv[1]
    with tempfile.TemporaryDirectory(prefix="logging-installed-") as directory:
        target = Path(directory)
        (target / "tests").mkdir()
        (target / "examples/python").mkdir(parents=True)
        for name in ("test_core.py", "test_python_example.py"):
            shutil.copyfile(root / "tests" / name, target / "tests" / name)
        shutil.copyfile(
            root / "examples/python/logging_example.py",
            target / "examples/python/logging_example.py",
        )
        probe = """
import importlib.metadata
from pathlib import Path
import sys
import estrellio_logging_lib
assert importlib.metadata.version('estrellio-logging-lib') == sys.argv[1]
assert Path(estrellio_logging_lib.__file__).resolve().is_relative_to(Path(sys.prefix).resolve())
print('Installed package:', estrellio_logging_lib.__file__)
"""
        subprocess.run([sys.executable, "-I", "-c", probe, expected_version], cwd=target, check=True)
        subprocess.run(
            [sys.executable, "-I", "-m", "pytest", "-q", "--import-mode=importlib", "tests"],
            cwd=target,
            check=True,
        )


if __name__ == "__main__":
    main()
