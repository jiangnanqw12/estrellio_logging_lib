"""Validate the contents of the Python release artifacts."""

from pathlib import Path
import sys
import tarfile
import zipfile


def main() -> None:
    dist = Path(sys.argv[1] if len(sys.argv) > 1 else "dist")
    wheels = list(dist.glob("*.whl"))
    sources = list(dist.glob("*.tar.gz"))
    assert len(wheels) == len(sources) == 1, "Expected one wheel and one sdist"
    assert wheels[0].name.endswith("-py3-none-any.whl")
    with zipfile.ZipFile(wheels[0]) as archive:
        names = set(archive.namelist())
        package_files = {name for name in names if ".dist-info/" not in name}
        assert package_files == {
            "estrellio_logging_lib/__init__.py",
            "estrellio_logging_lib/core.py",
        }, package_files
        assert any(name.endswith("/licenses/LICENSE") for name in names)
    with tarfile.open(sources[0]) as archive:
        names = {"/".join(Path(name).parts[1:]) for name in archive.getnames()}
        assert {
            "LICENSE", "README.md", "pyproject.toml",
            "src/estrellio_logging_lib/core.py",
            "examples/python/logging_example.py",
            "tests/test_core.py", "tests/test_python_example.py",
            "docs/EstrellioLogger Usage Manual.md",
        } <= names
        assert not any(name.startswith(("src/c/", "src/cpp/", "tests/native/")) for name in names)
    print("Python wheel and sdist contents verified.")


if __name__ == "__main__":
    main()
