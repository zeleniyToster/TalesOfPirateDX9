"""Доступ к базе SQLite.

Приложение читает базу, которую собирает :mod:`app.etl.ingest`,
поэтому соединение открывается на каждый запрос и закрывается сразу.
SQLite справляется с этим без проблем — запросы короткие и индексные.
"""

from __future__ import annotations

import sqlite3
from collections.abc import Iterator
from contextlib import contextmanager
from pathlib import Path

from .config import settings


def database_path() -> Path:
    path = settings.db_path
    if not path.is_file():
        raise FileNotFoundError(
            f"База {path} не найдена. Соберите её командой:\n"
            f"    python -m app.etl.ingest"
        )
    return path


@contextmanager
def connection() -> Iterator[sqlite3.Connection]:
    conn = sqlite3.connect(database_path(), check_same_thread=False)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA query_only = ON")
    try:
        yield conn
    finally:
        conn.close()


def query(sql: str, params: tuple = ()) -> list[sqlite3.Row]:
    with connection() as conn:
        return conn.execute(sql, params).fetchall()


def query_one(sql: str, params: tuple = ()) -> sqlite3.Row | None:
    with connection() as conn:
        return conn.execute(sql, params).fetchone()


def scalar(sql: str, params: tuple = ()):
    with connection() as conn:
        return conn.execute(sql, params).fetchone()[0]