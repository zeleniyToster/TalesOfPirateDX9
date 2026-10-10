"""Карточки предметов и монстров, ассортимент торговцев, статика.

Иконки и текстуры не копируются в проект: берутся из каталога,
который задаётся ``ARBAH_ASSETS_DIR`` и монтируется в контейнер
из того же тома, где лежит клиент.
"""

from __future__ import annotations

import json
import os
from pathlib import Path
from typing import Any

from fastapi import APIRouter, Depends, HTTPException, Query
from fastapi.responses import FileResponse

from .. import db
from ..etl.item_columns import EQUIP_SLOTS, JOB_RU
from ..schemas import Item, ItemList, Page
from .common import Paging, paging

router = APIRouter(prefix="/catalog", tags=["cards"])

# Ассеты отдаются отдельным роутером без префикса /catalog.
assets_router = APIRouter(prefix="/assets", tags=["assets"])

ICON_TYPES = (".png", ".bmp", ".tga", ".jpg", ".jpeg", ".dds")


@assets_router.get("/icon/{name}", summary="Иконка предмета")
def serve_icon(name: str) -> FileResponse:
    """Отдаёт иконку, подбирая расширение.

    В каталоге 4167 .png, но есть ещё .tga и .bmp, поэтому расширение
    ищется на диске, а не предполагается на стороне фронтенда.
    Имя приходит без расширения, как оно лежит в поле ``icon``
    таблицы ``ItemInfo.txt``.
    """
    if "/" in name or "\\" in name or ".." in name:
        raise HTTPException(status_code=400, detail="Недопустимое имя")

    path = _find_asset("icon", name)
    if path is None:
        raise HTTPException(status_code=404, detail=f"Иконка {name} не найдена")

    return FileResponse(
        path,
        media_type="image/png" if path.suffix.lower() == ".png" else None,
        headers={"Cache-Control": "public, max-age=604800"},
    )


def assets_dir() -> Path:
    """Каталог с ресурсами клиента (иконки, текстуры).

    Порядок поиска: переменная окружения, затем типовые места.
    В контейнере путь всегда задаётся явно через ``ARBAH_ASSETS_DIR``,
    локально удобно ничего не настраивать — ресурсы лежат рядом
    с репозиторием в ``Client/texture``.
    """
    raw = os.environ.get("ARBAH_ASSETS_DIR")
    if raw:
        return Path(raw)

    here = Path(__file__).resolve()
    project = here.parents[3]
    candidates = [
        project / "assets",             # локальная копия рядом с проектом
        project.parent / "Client" / "texture",   # репозиторий TOP
        project / "Client" / "texture",
    ]
    for candidate in candidates:
        if candidate.is_dir():
            return candidate
    return candidates[0]


def _find_asset(subdir: str, name: str) -> Path | None:
    """Ищет ресурс по имени, перебирая расширения и подкаталоги."""
    root = assets_dir() / subdir
    if not root.is_dir() or not name:
        return None
    base = Path(name).stem
    for ext in ICON_TYPES:
        direct = root / f"{base}{ext}"
        if direct.is_file():
            return direct
    return None


# --------------------------------------------------------------- предметы
def _csv(value: str | None) -> list[int]:
    if not value:
        return []
    out = []
    for part in value.split(","):
        part = part.strip()
        if part:
            try:
                out.append(int(part))
            except ValueError:
                continue
    return out


def item_detail(item_id: int) -> dict[str, Any]:
    row = db.query_one("SELECT * FROM items WHERE item_id = ?", (item_id,))
    if row is None:
        raise HTTPException(status_code=404, detail=f"Предмет {item_id} не найден")

    try:
        bonuses = json.loads(row["bonuses"] or "[]")
    except json.JSONDecodeError:
        bonuses = []

    jobs = _csv(row["jobs"])
    slots = _csv(row["equip_slots"])

    sold_by = [
        {
            "npc_id": r["npc_id"],
            "npc_name": r["name"],
            "location": r["location"],
            "region": r["region"],
            "x": r["x"],
            "y": r["y"],
        }
        for r in db.query(
            "SELECT n.npc_id, n.name, n.location, n.region, n.x, n.y "
            "FROM shop_items s JOIN npcs n ON n.npc_id = s.npc_id "
            "WHERE s.item_id = ? ORDER BY n.name",
            (item_id,),
        )
    ]

    return {
        "item_id": row["item_id"],
        "name": row["name"],
        "icon": row["icon"],
        "icon_url": f"/assets/icon/{row['icon']}.png" if row["icon"] else None,
        "type_id": row["type_id"],
        "type_name": row["type_name"],
        "description": row["description"],
        "price": row["price"],
        "pile_max": row["pile_max"],
        "required_level": row["required_level"],
        "forge_level": row["forge_level"],
        "forge_steady": row["forge_steady"],
        "set_id": row["set_id"],
        "prefix_chance": row["prefix_chance"],
        "endurance": row["endurance"],
        "holes": row["holes"],
        "use_effect": row["use_effect"] if row["use_effect"] != "0" else "",
        "flags": {
            "trade": bool(row["is_trade"]),
            "pickup": bool(row["is_pickup"]),
            "throw": bool(row["is_throw"]),
            "destroy": bool(row["is_delete"]),
            "instantiated": bool(row["instantiated"]),
            "exclusive": bool(row["is_exclusive"]),
        },
        "jobs": [JOB_RU.get(j, str(j)) for j in jobs],
        "job_ids": jobs,
        "equip_slots": [EQUIP_SLOTS[s] if 0 <= s < len(EQUIP_SLOTS) else f"Слот {s}"
                        for s in slots],
        "body_sizes": _csv(row["body_sizes"]),
        "bonuses": bonuses,
        "sold_by": sold_by,
        "gm_command": f"make {row['item_id']},1",
    }


@router.get("/items/{item_id}/full", summary="Полная карточка предмета")
def get_item_full(item_id: int) -> dict[str, Any]:
    return item_detail(item_id)


@router.get("/items/{item_id}/sold-by", summary="У кого продаётся предмет")
def item_sold_by(item_id: int) -> list[dict[str, Any]]:
    return item_detail(item_id)["sold_by"]


# ---------------------------------------------------------------- монстры
@router.get("/monsters/{monster_id}/full", summary="Карточка монстра из monsterlist")
def monster_detail(monster_id: int) -> dict[str, Any]:
    """Карточка записи из ``monsterlist.txt``.

    У этого списка своя нумерация (1..352), не совпадающая с ID из
    ``CreateCha``, поэтому карточка собирается через поиск существа
    по имени: так к ней подтягиваются точки спавна и регионы.
    """
    row = db.query_one("SELECT * FROM monsters WHERE monster_id = ?", (monster_id,))
    if row is None:
        raise HTTPException(status_code=404, detail=f"Монстр {monster_id} не найден")

    creature = db.query_one(
        "SELECT * FROM creatures WHERE LOWER(name) = LOWER(?) "
        "ORDER BY creature_id LIMIT 1",
        (row["name"],),
    )

    payload: dict[str, Any] = {
        "monster_id": monster_id,
        "name": row["name"],
        "level": row["level"],
        "region": row["region"],
        "x": row["x"],
        "y": row["y"],
        "creature_id": creature["creature_id"] if creature else None,
        "short_name": creature["short_name"] if creature else "",
    }

    if creature is not None:
        full = creature_detail(creature["creature_id"])
        payload["spawn_count"] = full["spawn_count"]
        payload["areas"] = full["areas"]
        payload["respawns"] = full["respawns"]
        payload["spawns"] = full["spawns"][:400]
        payload["spawns_truncated"] = full["spawns_truncated"]
    else:
        payload["spawn_count"] = 0
        payload["areas"] = []
        payload["respawns"] = []
        payload["spawns"] = []
        payload["spawns_truncated"] = False

    return payload


@router.get("/creatures/{creature_id}/full", summary="Карточка существа")
def creature_detail(creature_id: int) -> dict[str, Any]:
    creature = db.query_one(
        "SELECT * FROM creatures WHERE creature_id = ?", (creature_id,)
    )
    if creature is None:
        raise HTTPException(status_code=404, detail=f"Существо {creature_id} не найдено")

    spawns = [
        {
            "region": r["region"],
            "map_file": r["map_file"],
            "x": r["x"],
            "y": r["y"],
            "respawn": r["respawn"],
        }
        for r in db.query(
            "SELECT * FROM monster_spawns WHERE char_id = ? "
            "ORDER BY region, x, y",
            (creature_id,),
        )
    ]

    # Монстр не стоит на месте: CreateCha задаёт точку и направление,
    # а существо бродит вокруг неё. Поэтому показываем не отдельные
    # точки, а рамку, внутри которой они живут, — так же, как на
    # piratia.arbah.ru ("20/19 - 522/527").
    #
    # Координаты в игре хранятся в сотых долях тайла, поэтому рамка
    # делится на 100: 2043 -> 20, 52275 -> 522.
    groups: dict[str, dict[str, Any]] = {}
    for s in spawns:
        key = s["region"] or "?"
        g = groups.setdefault(
            key,
            {"region": key, "count": 0, "min_x": 0, "max_x": 0,
             "min_y": 0, "max_y": 0, "respawns": set()},
        )
        x, y = s["x"], s["y"]
        if g["count"] == 0:
            g["min_x"] = g["max_x"] = x
            g["min_y"] = g["max_y"] = y
        else:
            g["min_x"] = min(g["min_x"], x)
            g["max_x"] = max(g["max_x"], x)
            g["min_y"] = min(g["min_y"], y)
            g["max_y"] = max(g["max_y"], y)
        g["count"] += 1
        g["respawns"].add(s["respawn"])

    areas = []
    for g in groups.values():
        areas.append(
            {
                "region": g["region"],
                "count": g["count"],
                "area": (
                    f"{g['min_x'] // 100}/{g['min_y'] // 100}"
                    f" - {g['max_x'] // 100}/{g['max_y'] // 100}"
                ),
                "respawns": sorted(g["respawns"]),
            }
        )
    areas.sort(key=lambda a: -a["count"])

    # Уровень есть только в monsterlist.txt, у которого своя нумерация,
    # поэтому ищем существо по имени.
    level = None
    for r in db.query(
        "SELECT level FROM monsters "
        "WHERE LOWER(name) = LOWER(?) OR LOWER(name) = LOWER(?) LIMIT 1",
        (creature["name"], creature["short_name"] or ""),
    ):
        level = r["level"]

    # Картинка в клиенте называется по номеру модели существа.
    image_url = None
    if _find_asset("character", str(creature_id)):
        image_url = f"/assets/character/{creature_id}"

    return {
        "creature_id": creature["creature_id"],
        "name": creature["name"],
        "short_name": creature["short_name"],
        "model_type": creature["model_type"],
        "logic_type": creature["logic_type"],
        "level": level,
        "image_url": image_url,
        "spawn_count": len(spawns),
        "areas": areas,
        "respawns": sorted({s["respawn"] for s in spawns}),
        "spawns": spawns[:400],
        "spawns_truncated": len(spawns) > 400,
    }


@router.get("/creatures/search", summary="Поиск существ по имени")
def search_creatures(
    q: str = Query(..., min_length=1),
    limit: int = Query(20, ge=1, le=100),
    spawned_only: bool = Query(True, description="Только те, кто реально спавнится"),
) -> list[dict[str, Any]]:
    # Скобки обязательны: без них «a OR b AND spawn_count>0» из-за
    # приоритета операторов пропускает записи, найденные по имени.
    sql = (
        "SELECT c.creature_id, c.name, c.short_name, c.logic_type, "
        "       (SELECT COUNT(*) FROM monster_spawns s "
        "        WHERE s.char_id = c.creature_id) AS spawn_count "
        "FROM creatures c "
        "WHERE (LOWER(c.name) LIKE ? OR LOWER(c.short_name) LIKE ?)"
    )
    if spawned_only:
        sql += " AND spawn_count > 0"
    sql += " ORDER BY spawn_count DESC, c.name LIMIT ?"

    return [
        dict(r)
        for r in db.query(sql, (f"%{q.lower()}%", f"%{q.lower()}%", limit))
    ]


# --------------------------------------------------------------- магазины
@router.get("/npcs/{npc_id}/shop", summary="Ассортимент NPC")
def npc_shop(
    npc_id: int,
    pg: Paging = Depends(paging),
) -> dict[str, Any]:
    npc = db.query_one("SELECT * FROM npcs WHERE npc_id = ?", (npc_id,))
    if npc is None:
        raise HTTPException(status_code=404, detail=f"NPC {npc_id} не найден")

    total = db.scalar(
        "SELECT COUNT(*) FROM shop_items WHERE npc_id = ?", (npc_id,)
    )
    rows = db.query(
        "SELECT i.item_id, i.name, i.icon, i.price, i.required_level, i.type_name "
        "FROM shop_items s JOIN items i ON i.item_id = s.item_id "
        "WHERE s.npc_id = ? ORDER BY i.name LIMIT ? OFFSET ?",
        (npc_id, pg.page_size, pg.offset),
    )

    image_url = None
    if _find_asset("character", str(npc["npc_id"])):
        image_url = f"/assets/character/{npc['npc_id']}"

    return {
        "npc": {
            "npc_id": npc["npc_id"],
            "name": npc["name"],
            "location": npc["location"],
            "region": npc["region"],
            "x": npc["x"],
            "y": npc["y"],
            "dialogue": npc["dialogue"],
            "image_url": image_url,
        },
        "page": pg.build(total),
        "items": [
            {
                "item_id": r["item_id"],
                "name": r["name"],
                "icon": r["icon"],
                "icon_url": f"/assets/icon/{r['icon']}.png" if r["icon"] else None,
                "price": r["price"],
                "required_level": r["required_level"],
                "type_name": r["type_name"],
                "gm_command": f"make {r['item_id']},1",
            }
            for r in rows
        ],
    }


# ---------------------------------------------------------------- ассеты
@router.get("/items/{item_id}/icon-file", include_in_schema=False)
def item_icon_file(item_id: int) -> FileResponse:
    row = db.query_one("SELECT icon FROM items WHERE item_id = ?", (item_id,))
    if row is None or not row["icon"]:
        raise HTTPException(status_code=404, detail="Иконка не найдена")
    path = _find_asset("icon", row["icon"])
    if path is None:
        raise HTTPException(status_code=404, detail="Файл иконки отсутствует в ресурсах")
    return FileResponse(path)