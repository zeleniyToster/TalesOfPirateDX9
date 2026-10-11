"""Обслуживание базы: пересборка из игровых таблиц по требованию.

Зачем это нужно: база собирается из ``server/GameServer/resource``,
который подключен томом. После правки предметов, дропа или расписания
данжей на сервере нужно пересобрать базу, но пересобирать образ или
перезапускать контейнер неудобно — достаточно дёрнуть один эндпоинт.

Эндпоинт не защищён авторизацией и рассчитан на локальную сеть:
он только перечитывает файлы и перезаписывает SQLite-файл, данные
игры не изменяет. Если справочник выставлен в интернет, закройте
маршрут обратным прокси.
"""

from __future__ import annotations

import threading
import time
from typing import Any

from fastapi import APIRouter, HTTPException

from .. import db
from ..config import settings

router = APIRouter(prefix="/admin", tags=["admin"])

_lock = threading.Lock()


def _build_in_progress() -> bool:
    return _lock.locked()


@router.get("/refresh/status", summary="Состояние пересборки базы")
def refresh_status() -> dict[str, Any]:
    """Когда база обновлялась в последний раз и что в ней лежит.

    Эндпоинт должен отвечать и до первой пересборки, поэтому
    отсутствие файла базы — не ошибка, а ``database_ready: false``.
    """
    if not settings.db_path.is_file():
        return {
            "in_progress": _build_in_progress(),
            "database_ready": False,
            "data_dir": str(settings.data_dir),
            "db_path": str(settings.db_path),
            "updated_at": "",
            "counts": {},
        }

    rows = db.query("SELECT key, value FROM meta")
    meta = {r["key"]: r["value"] for r in rows}

    return {
        "in_progress": _build_in_progress(),
        "database_ready": True,
        "data_dir": str(settings.data_dir),
        "db_path": str(settings.db_path),
        "updated_at": meta.get("built_at", ""),
        "counts": meta,
    }


@router.post("/refresh", summary="Пересобрать базу из игровых таблиц")
def refresh() -> dict[str, Any]:
    """Перечитывает ``ARBAH_DATA_DIR`` и пересобирает SQLite.

    Одновременные вызовы отсекаются: два параллельных импорта
    записали бы в один файл разные данные. Второй запрос получает
    409, а не молча ждёт.
    """
    from ..etl.ingest import build

    if not _lock.acquire(blocking=False):
        raise HTTPException(
            status_code=409, detail="Пересборка уже выполняется, попробуйте позже"
        )

    started = time.time()
    try:
        if not settings.data_dir.is_dir():
            raise HTTPException(
                status_code=500,
                detail=f"Каталог с игровыми таблицами не найден: {settings.data_dir}",
            )
        counts = build(settings.data_dir, settings.db_path)
    finally:
        _lock.release()

    return {
        "status": "ok",
        "data_dir": str(settings.data_dir),
        "seconds": round(time.time() - started, 2),
        "counts": counts,
    }