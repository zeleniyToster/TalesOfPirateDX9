"""Конфигурация приложения.

Все параметры читаются из переменных окружения, чтобы один и тот же
образ работал и под Windows (venv), и в Docker-контейнере.
"""

from __future__ import annotations

import os
from dataclasses import dataclass, field
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent


# backend/app -> backend -> arbah_project
PROJECT_DIR = BASE_DIR.parent


def _env_path(name: str, default: Path) -> Path:
    raw = os.environ.get(name)
    return Path(raw) if raw else default


@dataclass(frozen=True)
class Settings:
    # Каталог с исходными .txt-таблицами игры (монтируется в контейнер).
    data_dir: Path = field(
        default_factory=lambda: _env_path("ARBAH_DATA_DIR", PROJECT_DIR / "data")
    )
    # Файл базы SQLite, собирается ETL-пайплайном.
    db_path: Path = field(
        default_factory=lambda: _env_path("ARBAH_DB_PATH", PROJECT_DIR / "data" / "arbah.db")
    )
    # Каталог статики фронтенда.
    frontend_dir: Path = field(
        default_factory=lambda: _env_path("ARBAH_FRONTEND_DIR", PROJECT_DIR / "frontend")
    )

    # Необязательное подключение к боевой MSSQL.
    # Если ARBAH_MSSQL_DSN не задан, приложение работает только на SQLite.
    mssql_dsn: str | None = field(
        default_factory=lambda: os.environ.get("ARBAH_MSSQL_DSN")
    )

    api_prefix: str = "/api"
    page_size_default: int = 50
    page_size_max: int = 200


settings = Settings()