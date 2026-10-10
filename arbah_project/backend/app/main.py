"""Приложение FastAPI: собирает роутеры, отдаёт статику фронтенда."""

from __future__ import annotations

from fastapi import APIRouter, FastAPI
from fastapi.responses import FileResponse, JSONResponse
from fastapi.staticfiles import StaticFiles

from . import db
from .config import settings
from .routers import calculator, cards, catalog, levels
from .routers.cards import assets_dir

api = APIRouter(prefix=settings.api_prefix)
api.include_router(catalog.router)
api.include_router(cards.router)
api.include_router(levels.router)
api.include_router(calculator.router)


@api.get("/health", tags=["system"], summary="Проверка живости")
def health() -> object:
    """Отвечает всегда, даже если база недоступна, чтобы мониторинг
    отличал «сервис упал» от «данные не подгрузились»."""
    try:
        counts = {
            "items": db.scalar("SELECT COUNT(*) FROM items"),
            "monsters": db.scalar("SELECT COUNT(*) FROM monsters"),
            "npcs": db.scalar("SELECT COUNT(*) FROM npcs"),
            "creatures": db.scalar("SELECT COUNT(*) FROM creatures"),
            "spawns": db.scalar("SELECT COUNT(*) FROM monster_spawns"),
        }
        return {"status": "ok", "database": "ok", "counts": counts}
    except Exception as exc:  # noqa: BLE001 - health-check отвечает всегда
        return JSONResponse(
            status_code=503,
            content={"status": "degraded", "database": str(exc)},
        )


@api.get("/meta", tags=["system"], summary="Сведения о базе")
def meta() -> dict[str, str]:
    rows = db.query("SELECT key, value FROM meta")
    return {r["key"]: r["value"] for r in rows}


def create_app() -> FastAPI:
    app = FastAPI(
        title="Arbah — база данных Pirates of the Caribbean Online",
        description=(
            "Каталог предметов, монстров и NPC, таблица опыта "
            "и калькулятор боевых характеристик."
        ),
        version="1.0.0",
        docs_url=f"{settings.api_prefix}/docs",
        openapi_url=f"{settings.api_prefix}/openapi.json",
    )
    app.include_router(api)
    # Ассеты лежат вне /api: фронтенд запрашивает /assets/icon/<имя>.png
    app.include_router(cards.assets_router)

    if settings.frontend_dir.is_dir():
        app.mount("/static", StaticFiles(directory=settings.frontend_dir), name="static")

        @app.get("/", include_in_schema=False)
        def index() -> FileResponse:
            return FileResponse(settings.frontend_dir / "index.html")

    # Ресурсы клиента (иконки, текстуры). В контейнере приходят из
    # тома с клиентом, локально — из Client/texture рядом с репозиторием.
    # Иконки отдаёт динамический роутер (нужен подбор расширения),
    # остальное можно раздавать статикой.
    assets = assets_dir()
    if assets.is_dir():
        for sub in ("character", "item"):
            folder = assets / sub
            if folder.is_dir():
                app.mount(
                    f"/assets/{sub}", StaticFiles(directory=folder), name=f"assets-{sub}"
                )

    return app


app = create_app()