from __future__ import annotations

import logging
from pathlib import Path

from estrellio_logging_lib import init_logger


def main() -> None:
    log_path = Path("logs") / "demo.log"
    logger = init_logger("logging_demo", level=logging.WARNING, log_file_path=log_path)
    logger.info("hidden by default")
    logger.warning("warning message")
    logger.error("error message")


if __name__ == "__main__":
    main()
