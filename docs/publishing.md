# Publishing the Python package

The PyPI distribution is `estrellio-logging-lib`; the import name remains
`estrellio_logging_lib`. Python requires no native compilation. The wheel
contains only the Python package and distribution metadata. The sdist also
contains Python examples, tests, and documentation. Build the separate C/C++
libraries from the Git repository, not from the Python distribution.

## One-time account setup

At https://pypi.org/manage/account/publishing/ add a pending GitHub publisher:

| Field | Value |
| --- | --- |
| PyPI project name | `estrellio-logging-lib` |
| Repository owner | `jiangnanqw12` |
| Repository name | `estrellio_logging_lib` |
| Workflow name | `publish.yml` |
| Environment name | `pypi` |

The GitHub repository must have an environment named `pypi`. No PyPI token
is stored in GitHub or passed to the workflow. A pending publisher does not
reserve a name; stop and resolve any name or existing-version conflict.

## Validate before releasing

From the repository root, using a development environment:

```console
python -m pip install build twine pytest
python -m pytest -q tests
python -m build
python -m twine check --strict dist/*
python scripts/check_dist.py dist
```

Build from a clean checkout without stale `dist/` files. `python -m build`
creates the sdist, then builds the wheel from that sdist. Install the wheel
and pytest in a fresh virtual environment, then use its Python to run
`python scripts/verify_install.py 0.1.0` (substitute the release version).
This copies the tests outside the checkout without the source-path conftest,
checks the installed version and location, and tests the installed package
and copyable example. CI does this on Ubuntu and Windows with Python 3.10
and 3.12, and checks the native libraries separately with CMake/CTest.

## Publish a version

1. Update `project.version` in `pyproject.toml` and commit the release changes.
2. Push `main` and confirm its build and installed-package tests pass.
3. Confirm the PyPI publisher is configured before creating the release tag.
4. Create and push the matching tag, for example `v0.1.0` for version `0.1.0`.
5. Check the `publish.yml` run. Only a version tag enables publication, after
   all validation jobs pass. The publishing job alone has OIDC write permission.
6. Install the exact version from https://pypi.org/simple in a fresh environment
   and run the installed-package verification again.

If the name is unavailable or a distribution version already exists, stop;
do not automatically rename, overwrite, or silently skip existing artifacts.
For later code changes, release a new version. Ordinary branch pushes and pull
requests validate artifacts but do not publish them.
