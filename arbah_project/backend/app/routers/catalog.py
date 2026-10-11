"""Каталоги: предметы, монстры, NPC, области и карты."""

from __future__ import annotations

from datetime import datetime
from typing import Any

from fastapi import APIRouter, Depends, Query

from .. import db
from ..schemas import Area, Item, ItemList, MapOut, Monster, MonsterList, Npc, NpcList
from .common import Paging, order_by, paging

router = APIRouter(prefix="/catalog", tags=["catalog"])

ITEM_SORTS = {
    "id": "item_id",
    "name": "name",
    "level": "required_level",
    "price": "price",
    "type": "type_id",
}
MONSTER_SORTS = {"id": "monster_id", "name": "name", "level": "level"}
NPC_SORTS = {"id": "npc_id", "name": "name"}


def _item(row: Any) -> Item:
    return Item(
        item_id=row["item_id"],
        name=row["name"],
        icon=row["icon"],
        type_id=row["type_id"],
        type_name=row["type_name"],
        forge_level=row["forge_level"],
        price=row["price"],
        pile_max=row["pile_max"],
        jobs=[int(j) for j in (row["jobs"] or "").split(",") if j.strip()],
        required_level=row["required_level"],
    )


@router.get("/items", response_model=ItemList, summary="Каталог предметов")
def list_items(
    q: str | None = Query(None, description="Поиск по названию"),
    type_id: int | None = Query(None, description="Код типа предмета"),
    min_level: int | None = Query(None, ge=0, description="Минимальный уровень"),
    max_level: int | None = Query(None, ge=0, description="Максимальный уровень"),
    sort: str = Query("id", description=f"Поле сортировки: {', '.join(ITEM_SORTS)}"),
    desc: bool = Query(False, description="Сортировать по убыванию"),
    pg: Paging = Depends(paging),
) -> ItemList:
    where: list[str] = []
    args: list[Any] = []

    if q:
        where.append("(name LIKE ? OR CAST(item_id AS TEXT) LIKE ?)")
        args += [f"%{q}%", f"%{q}%"]
    if type_id is not None:
        where.append("type_id = ?")
        args.append(type_id)
    if min_level is not None:
        where.append("required_level >= ?")
        args.append(min_level)
    if max_level is not None:
        where.append("required_level <= ?")
        args.append(max_level)

    clause = (" WHERE " + " AND ".join(where)) if where else ""
    total = db.scalar(f"SELECT COUNT(*) FROM items{clause}", tuple(args))

    column = order_by(sort, ITEM_SORTS, "id")
    direction = "DESC" if desc else "ASC"
    rows = db.query(
        f"SELECT * FROM items{clause} ORDER BY {column} {direction}, item_id ASC "
        f"LIMIT ? OFFSET ?",
        tuple(args) + (pg.page_size, pg.offset),
    )
    return ItemList(page=pg.build(total), items=[_item(r) for r in rows])


@router.get("/items/types", summary="Справочник типов предметов")
def item_types() -> list[dict[str, Any]]:
    rows = db.query(
        "SELECT type_id AS id, type_name AS name, COUNT(*) AS count "
        "FROM items GROUP BY type_id, type_name ORDER BY count DESC"
    )
    return [dict(r) for r in rows]


@router.get("/items/{item_id}", response_model=Item, summary="Предмет по ID")
def get_item(item_id: int) -> Item:
    row = db.query_one("SELECT * FROM items WHERE item_id = ?", (item_id,))
    if row is None:
        from fastapi import HTTPException

        raise HTTPException(status_code=404, detail=f"Предмет {item_id} не найден")
    return _item(row)


@router.get("/monsters", response_model=MonsterList, summary="Каталог монстров")
def list_monsters(
    q: str | None = Query(None, description="Поиск по названию"),
    min_level: int | None = Query(None, ge=0),
    max_level: int | None = Query(None, ge=0),
    region: str | None = Query(None, description="Регион, например Ascaron"),
    sort: str = Query("id", description=f"Поле сортировки: {', '.join(MONSTER_SORTS)}"),
    desc: bool = Query(False),
    pg: Paging = Depends(paging),
) -> MonsterList:
    where: list[str] = []
    args: list[Any] = []

    if q:
        where.append("(name LIKE ? OR CAST(monster_id AS TEXT) LIKE ?)")
        args += [f"%{q}%", f"%{q}%"]
    if min_level is not None:
        where.append("level >= ?")
        args.append(min_level)
    if max_level is not None:
        where.append("level <= ?")
        args.append(max_level)
    if region:
        where.append("region = ?")
        args.append(region)

    clause = (" WHERE " + " AND ".join(where)) if where else ""
    total = db.scalar(f"SELECT COUNT(*) FROM monsters{clause}", tuple(args))
    column = order_by(sort, MONSTER_SORTS, "id")
    direction = "DESC" if desc else "ASC"
    rows = db.query(
        f"SELECT * FROM monsters{clause} ORDER BY {column} {direction}, monster_id ASC "
        f"LIMIT ? OFFSET ?",
        tuple(args) + (pg.page_size, pg.offset),
    )
    return MonsterList(
        page=pg.build(total),
        monsters=[
            Monster(
                monster_id=r["monster_id"],
                name=r["name"],
                level=r["level"],
                x=r["x"],
                y=r["y"],
                region=r["region"],
            )
            for r in rows
        ],
    )


@router.get("/monsters/regions", summary="Справочник регионов")
def monster_regions() -> list[dict[str, Any]]:
    rows = db.query(
        "SELECT region AS name, COUNT(*) AS count, MIN(level) AS min_level, "
        "MAX(level) AS max_level FROM monsters "
        "WHERE region IS NOT NULL AND region <> '' GROUP BY region ORDER BY count DESC"
    )
    return [dict(r) for r in rows]


@router.get("/npcs", response_model=NpcList, summary="Каталог NPC")
def list_npcs(
    q: str | None = Query(None, description="Поиск по названию"),
    region: str | None = Query(None),
    shop_only: bool = Query(False, description="Только торговцы"),
    sort: str = Query("id", description=f"Поле сортировки: {', '.join(NPC_SORTS)}"),
    desc: bool = Query(False),
    pg: Paging = Depends(paging),
) -> NpcList:
    where: list[str] = []
    args: list[Any] = []

    if q:
        where.append("(name LIKE ? OR CAST(npc_id AS TEXT) LIKE ?)")
        args += [f"%{q}%", f"%{q}%"]
    if region:
        where.append("region = ?")
        args.append(region)
    if shop_only:
        where.append("npc_id IN (SELECT npc_id FROM shop_items)")

    clause = (" WHERE " + " AND ".join(where)) if where else ""
    total = db.scalar(f"SELECT COUNT(*) FROM npcs{clause}", tuple(args))
    column = order_by(sort, NPC_SORTS, "id")
    direction = "DESC" if desc else "ASC"
    rows = db.query(
        f"SELECT * FROM npcs{clause} ORDER BY {column} {direction}, npc_id ASC "
        f"LIMIT ? OFFSET ?",
        tuple(args) + (pg.page_size, pg.offset),
    )
    return NpcList(
        page=pg.build(total),
        npcs=[
            Npc(
                npc_id=r["npc_id"],
                name=r["name"],
                location=r["location"],
                x=r["x"],
                y=r["y"],
                region=r["region"],
            )
            for r in rows
        ],
    )


@router.get("/npcs/regions", summary="Справочник регионов NPC")
def npc_regions() -> list[dict[str, Any]]:
    rows = db.query(
        "SELECT region AS name, COUNT(*) AS count FROM npcs "
        "WHERE region IS NOT NULL AND region <> '' GROUP BY region ORDER BY count DESC"
    )
    return [dict(r) for r in rows]


@router.get("/areas", response_model=list[Area], summary="Области карт")
def list_areas() -> list[Area]:
    rows = db.query("SELECT * FROM areas ORDER BY area_id")
    return [
        Area(
            area_id=r["area_id"],
            name=r["name"],
            r=r["r"],
            g=r["g"],
            b=r["b"],
            is_city=bool(r["is_city"]),
        )
        for r in rows
    ]


@router.get("/maps", response_model=list[MapOut], summary="Карты (регионы)")
def list_maps() -> list[MapOut]:
    rows = db.query("SELECT * FROM maps ORDER BY map_id")
    return [
        MapOut(
            map_id=r["map_id"],
            file=r["file"],
            name=r["name"],
            x=r["x"],
            y=r["y"],
        )
        for r in rows
    ]


def _open_now(hours: str, hour: int) -> bool:
    """Открыт ли данж в указанный час.

    Пустой список часов означает свободный вход. Время серверное
    локальное — то же, что использует ``can_open_entry_*`` через
    ``GetNowTime()``, поэтому сверка идёт без поправки на часовой
    пояс: хости сам выставлен на нужный.
    """
    if not hours:
        return True
    return hour in {int(h) for h in hours.split(",") if h.strip()}


@router.get("/dungeons", summary="Расписание входа в данжи")
def list_dungeons() -> dict[str, Any]:
    """Расписание из ``<карта>/ctrl.lua`` плюс отметка «открыт сейчас».

    Карты, у которых нет функции ``can_open_entry_<карта>``, в
    таблицу не попадают: их вход ничем не ограничен, и перечислять
    их бессмысленно — в ответе они отмечены ``always_open``.
    """
    hour = datetime.now().hour

    rows = db.query(
        "SELECT d.map_file, d.always_open, d.hours, "
        "       (SELECT name FROM maps m WHERE m.file = d.map_file) AS name "
        "FROM dungeons d ORDER BY d.always_open, d.map_file"
    )

    dungeons = [
        {
            "map_file": r["map_file"],
            "name": r["name"] or r["map_file"],
            "always_open": bool(r["always_open"]),
            "hours": [int(h) for h in (r["hours"] or "").split(",") if h.strip()],
            "open_now": bool(r["always_open"]) or _open_now(r["hours"] or "", hour),
        }
        for r in rows
    ]

    return {
        "hour": hour,
        "total": len(dungeons),
        "scheduled": sum(1 for d in dungeons if not d["always_open"]),
        "dungeons": dungeons,
    }