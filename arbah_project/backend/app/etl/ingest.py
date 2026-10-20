"""Сборка SQLite-базы из игровых таблиц.

Запуск идемпотентен: база пересоздаётся целиком, так что повторный
импорт после правки ``.txt`` безопасен.

Использование::

    python -m app.etl.ingest [каталог_с_таблицами]
"""

from __future__ import annotations

import json
import os
import re
import sqlite3
import sys
from datetime import datetime
from pathlib import Path
from typing import Iterable

from . import cha_columns as cha
from . import dungeon_schedule, game_scripts, parsers

# Файлы, без которых импорт бессмыслен.
REQUIRED_FILES = ("ItemInfo.txt", "monsterlist.txt", "npclist.txt")

SCHEMA = """
PRAGMA journal_mode = WAL;

DROP TABLE IF EXISTS items;
CREATE TABLE items (
    item_id        INTEGER PRIMARY KEY,
    name           TEXT NOT NULL,
    icon           TEXT,
    type_id        INTEGER,
    type_name      TEXT,
    forge_level    INTEGER,
    forge_steady   INTEGER,
    set_id         INTEGER,
    prefix_chance  INTEGER,
    price          INTEGER,
    pile_max       INTEGER,
    required_level INTEGER,
    jobs           TEXT,
    equip_slots    TEXT,
    body_sizes     TEXT,
    is_trade       INTEGER,
    is_pickup      INTEGER,
    is_throw       INTEGER,
    is_delete      INTEGER,
    instantiated   INTEGER,
    is_exclusive   INTEGER,
    endurance      INTEGER,
    holes          INTEGER,
    use_effect     TEXT,
    description    TEXT,
    bonuses        TEXT
);
CREATE INDEX idx_items_name    ON items(name);
CREATE INDEX idx_items_type    ON items(type_id);
CREATE INDEX idx_items_level   ON items(required_level);

DROP TABLE IF EXISTS item_bonuses;
CREATE TABLE item_bonuses (
    item_id  INTEGER NOT NULL,
    field    TEXT    NOT NULL,
    title    TEXT    NOT NULL,
    min_val  INTEGER NOT NULL,
    max_val  INTEGER NOT NULL,
    coef_min INTEGER NOT NULL,
    coef_max INTEGER NOT NULL
);
CREATE INDEX idx_item_bonuses_item ON item_bonuses(item_id);

DROP TABLE IF EXISTS monsters;
CREATE TABLE monsters (
    monster_id INTEGER PRIMARY KEY,
    name       TEXT NOT NULL,
    level      INTEGER,
    x          INTEGER,
    y          INTEGER,
    region     TEXT
);
CREATE INDEX idx_monsters_name   ON monsters(name);
CREATE INDEX idx_monsters_level  ON monsters(level);
CREATE INDEX idx_monsters_region ON monsters(region);

DROP TABLE IF EXISTS creatures;
CREATE TABLE creatures (
    creature_id INTEGER PRIMARY KEY,
    name        TEXT NOT NULL,
    short_name  TEXT,
    model_type  INTEGER,
    logic_type  INTEGER
);
CREATE INDEX idx_creatures_name ON creatures(name);

DROP TABLE IF EXISTS monster_spawns;
CREATE TABLE monster_spawns (
    char_id   INTEGER NOT NULL,
    map_file  TEXT    NOT NULL,
    region    TEXT,
    x         INTEGER,
    y         INTEGER,
    angle     INTEGER,
    respawn   INTEGER
);
CREATE INDEX idx_spawns_char ON monster_spawns(char_id);
CREATE INDEX idx_spawns_map  ON monster_spawns(map_file);

DROP TABLE IF EXISTS npcs;
CREATE TABLE npcs (
    npc_id   INTEGER PRIMARY KEY,
    name     TEXT NOT NULL,
    location TEXT,
    x        INTEGER,
    y        INTEGER,
    region   TEXT,
    dialogue TEXT
);
CREATE INDEX idx_npcs_name ON npcs(name);
CREATE INDEX idx_npcs_region ON npcs(region);

DROP TABLE IF EXISTS shop_items;
CREATE TABLE shop_items (
    npc_id  INTEGER NOT NULL,
    item_id INTEGER NOT NULL,
    PRIMARY KEY (npc_id, item_id)
);
CREATE INDEX idx_shop_items_item ON shop_items(item_id);

DROP TABLE IF EXISTS areas;
CREATE TABLE areas (
    area_id  INTEGER PRIMARY KEY,
    name     TEXT NOT NULL,
    r        INTEGER,
    g        INTEGER,
    b        INTEGER,
    is_city  INTEGER
);
CREATE INDEX idx_areas_name ON areas(name);

DROP TABLE IF EXISTS maps;
CREATE TABLE maps (
    map_id INTEGER PRIMARY KEY,
    file   TEXT,
    name   TEXT NOT NULL,
    x      INTEGER,
    y      INTEGER
);

DROP TABLE IF EXISTS ship_items;
CREATE TABLE ship_items (
    ship_item_id INTEGER PRIMARY KEY,
    name         TEXT NOT NULL,
    model        INTEGER,
    price        INTEGER
);

DROP TABLE IF EXISTS level_costs;
CREATE TABLE level_costs (
    level     INTEGER PRIMARY KEY,
    exp_total INTEGER NOT NULL,
    exp_cost  INTEGER NOT NULL
);

DROP TABLE IF EXISTS creature_stats;
CREATE TABLE creature_stats (
    creature_id INTEGER PRIMARY KEY,
    level       INTEGER,
    max_hp      INTEGER,
    max_sp      INTEGER,
    min_atk     INTEGER,
    max_atk     INTEGER,
    phys_res    INTEGER,
    defense     INTEGER,
    hit         INTEGER,
    dodge       INTEGER,
    atk_speed   INTEGER,
    move_speed  INTEGER,
    exp         INTEGER
);
CREATE INDEX idx_creature_stats_level ON creature_stats(level);

DROP TABLE IF EXISTS creature_drops;
CREATE TABLE creature_drops (
    creature_id INTEGER NOT NULL,
    slot        INTEGER NOT NULL,
    item_id     INTEGER NOT NULL,
    rate        INTEGER NOT NULL,
    PRIMARY KEY (creature_id, slot)
);
CREATE INDEX idx_creature_drops_item    ON creature_drops(item_id);
CREATE INDEX idx_creature_drops_creature ON creature_drops(creature_id);

DROP TABLE IF EXISTS dungeons;
CREATE TABLE dungeons (
    map_file     TEXT PRIMARY KEY,
    always_open  INTEGER NOT NULL,
    hours        TEXT,
    first_sec    INTEGER NOT NULL DEFAULT 0,
    interval_sec INTEGER NOT NULL DEFAULT 0,
    entry_sec    INTEGER NOT NULL DEFAULT 0,
    life_sec     INTEGER NOT NULL DEFAULT 0
);

DROP TABLE IF EXISTS meta;
CREATE TABLE meta (
    key   TEXT PRIMARY KEY,
    value TEXT
);
"""


def resolve_data_dir(argument: str | None = None) -> Path:
    """Находит каталог с таблицами.

    Порядок: явный аргумент -> ``ARBAH_DATA_DIR`` -> локальный
    ``arbah_project/data`` -> ресурсы GameServer в этом же репозитории.
    """
    candidates: list[Path] = []
    if argument:
        candidates.append(Path(argument))
    if env := __import__("os").environ.get("ARBAH_DATA_DIR"):
        candidates.append(Path(env))

    here = Path(__file__).resolve()
    candidates.append(here.parents[3] / "data")
    candidates.append(here.parents[3].parent / "server" / "GameServer" / "resource")

    for candidate in candidates:
        if candidate.is_dir() and all((candidate / f).is_file() for f in REQUIRED_FILES):
            return candidate

    raise SystemExit(
        "Не найден каталог с игровыми таблицами. Проверены:\n  "
        + "\n  ".join(str(c) for c in candidates)
    )


def _insert_many(conn: sqlite3.Connection, sql: str, rows: Iterable[tuple]) -> int:
    rows = list(rows)
    conn.executemany(sql, rows)
    return len(rows)


# Порядок кортежей в item_rows() обязан совпадать с порядком колонок
# в схеме items; проверяется на старте, чтобы ошибка не всплыла
# в середине загрузки 6000 предметов.
ITEM_COLUMNS = 26


def _parse_level_costs(path: Path) -> list[tuple[int, int, int]]:
    """``character_lvup.txt`` -> (уровень, накопленный опыт, стоимость)."""
    totals: dict[int, int] = {}
    for row in parsers.read_table(path):
        level = parsers.to_int(parsers._cell(row, 0))
        totals[level] = parsers.to_int(parsers._cell(row, 2))

    out: list[tuple[int, int, int]] = []
    previous = 0
    for level in sorted(totals):
        total = totals[level]
        out.append((level, total, max(0, total - previous)))
        previous = total
    return out


def build(data_dir: Path, db_path: Path) -> dict[str, int]:
    """Пересобирает базу ``db_path`` из таблиц в ``data_dir``."""
    db_path.parent.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(db_path)
    counts: dict[str, int] = {}

    try:
        conn.executescript(SCHEMA)

        item_types = (
            parsers.parse_itemtype(data_dir / "itemtype.txt")
            if (data_dir / "itemtype.txt").is_file()
            else {}
        )

        def item_rows():
            for item in parsers.parse_items(data_dir / "ItemInfo.txt"):
                yield (
                    item["item_id"],
                    item["name"],
                    item["icon"],
                    item["type_id"],
                    item_types.get(item["type_id"], ""),
                    item["forge_level"],
                    item["forge_steady"],
                    item["set_id"],
                    item["prefix_chance"],
                    item["price"],
                    item["pile_max"],
                    item["required_level"],
                    ",".join(str(j) for j in item["jobs"]),
                    ",".join(str(s) for s in item["equip_slots"]),
                    ",".join(str(b) for b in item["body_sizes"]),
                    int(item["is_trade"]),
                    int(item["is_pickup"]),
                    int(item["is_throw"]),
                    int(item["is_delete"]),
                    int(item["instantiated"]),
                    int(item["exclusive"]),
                    item["endurance"],
                    item["holes"],
                    item["use_effect"],
                    item["description"],
                    json.dumps(item["bonuses"], ensure_ascii=False),
                )

        counts["items"] = _insert_many(
            conn,
            "INSERT INTO items VALUES ({})".format(
                ", ".join("?" * ITEM_COLUMNS)
            ),
            item_rows(),
        )

        # Бонусы держим отдельной таблицей: по ним понадобится
        # искать предметы по конкретному бонусу.
        def bonus_rows():
            for item in parsers.parse_items(data_dir / "ItemInfo.txt"):
                for bonus in item["bonuses"]:
                    yield (item["item_id"], bonus["field"],
                           bonus["title"], bonus["min"], bonus["max"],
                           bonus["coef_min"], bonus["coef_max"])

        counts["item_bonuses"] = _insert_many(
            conn,
            "INSERT INTO item_bonuses VALUES (?,?,?,?,?,?,?)",
            bonus_rows(),
        )

        counts["monsters"] = _insert_many(
            conn,
            "INSERT INTO monsters VALUES (?,?,?,?,?,?)",
            (
                (m["monster_id"], m["name"], m["level"], m["x"], m["y"], m["region"])
                for m in parsers.parse_monsters(data_dir / "monsterlist.txt")
            ),
        )

        npc_rows = list(parsers.parse_npcs(data_dir / "npclist.txt"))
        npc_names = {n["npc_id"]: n["name"] for n in npc_rows}

        npc_script_dir = game_scripts.find_npc_script_dir(data_dir)

        dialogues = (
            game_scripts.shop_dialogue(npc_script_dir, npc_names)
            if npc_script_dir is not None
            else {}
        )

        def npc_rows_iter():
            for n in npc_rows:
                yield (
                    n["npc_id"], n["name"], n["location"], n["x"], n["y"],
                    n["region"], dialogues.get(n["npc_id"], ""),
                )

        counts["npcs"] = _insert_many(
            conn,
            "INSERT INTO npcs VALUES (?,?,?,?,?,?,?)",
            npc_rows_iter(),
        )

        if game_scripts.find_spawns_dir(data_dir) is not None:
            counts["monster_spawns"] = _insert_many(
                conn,
                "INSERT INTO monster_spawns VALUES (?,?,?,?,?,?,?)",
                game_scripts.parse_spawns(data_dir),
            )

        if npc_script_dir is not None:
            counts["shop_items"] = _insert_many(
                conn,
                "INSERT OR IGNORE INTO shop_items VALUES (?,?)",
                game_scripts.parse_shops(npc_script_dir, npc_names),
            )

        if (data_dir / "CharacterInfo.txt").is_file():
            counts["creatures"] = _insert_many(
                conn,
                "INSERT INTO creatures VALUES (?,?,?,?,?)",
                (
                    (c["creature_id"], c["name"], c["short_name"],
                     c["model_type"], c["logic_type"])
                    for c in parsers.parse_creatures(data_dir / "CharacterInfo.txt")
                ),
            )

            # Боевые характеристики читаются из тех же строк, поэтому
            # файл открывается ещё раз — держать его в памяти ради
            # одного разбора дороже, чем повторное чтение с диска.
            counts["creature_stats"] = _insert_many(
                conn,
                "INSERT OR REPLACE INTO creature_stats VALUES ({})".format(
                    ", ".join("?" * (1 + len(cha.STAT_FIELDS)))
                ),
                parsers.parse_creature_stats(data_dir / "CharacterInfo.txt"),
            )

            counts["creature_drops"] = _insert_many(
                conn,
                "INSERT OR REPLACE INTO creature_drops VALUES (?,?,?,?)",
                parsers.parse_creature_drops(data_dir / "CharacterInfo.txt"),
            )

        # Расписание данжей лежит в <карта>/ctrl.lua рядом с ресурсами
        # GameServer. Если каталог данных отдельный (ARBAH_DATA_DIR=/srv/arbah),
        # берём карты из ARBAH_MAP_DIR, иначе ищем рядом с таблицами.
        map_dir = data_dir
        if not any(map_dir.glob("*/ctrl.lua")):
            env_map = os.environ.get("ARBAH_MAP_DIR")
            if env_map:
                candidate = Path(env_map)
                if any(candidate.glob("*/ctrl.lua")):
                    map_dir = candidate

        counts["dungeons"] = _insert_many(
            conn,
            "INSERT OR REPLACE INTO dungeons VALUES (?,?,?,?,?,?,?)",
            (
                (
                    d["map_file"],
                    int(d["always_open"]),
                    d["hours"],
                    d["first_sec"],
                    d["interval_sec"],
                    d["entry_sec"],
                    d["life_sec"],
                )
                for d in dungeon_schedule.parse_dungeon_schedules(map_dir)
            ),
        )

        if (data_dir / "AreaSet.txt").is_file():
            counts["areas"] = _insert_many(
                conn,
                "INSERT INTO areas VALUES (?,?,?,?,?,?)",
                (
                    (a["area_id"], a["name"], a["r"], a["g"], a["b"], int(a["is_city"]))
                    for a in parsers.parse_areas(data_dir / "AreaSet.txt")
                ),
            )
        if (data_dir / "mapinfo.txt").is_file():
            counts["maps"] = _insert_many(
                conn,
                "INSERT INTO maps VALUES (?,?,?,?,?)",
                (
                    (m["map_id"], m["file"], m["name"], m["x"], m["y"])
                    for m in parsers.parse_maps(data_dir / "mapinfo.txt")
                ),
            )
        if (data_dir / "ShipItemInfo.txt").is_file():
            counts["ship_items"] = _insert_many(
                conn,
                "INSERT INTO ship_items VALUES (?,?,?,?)",
                (
                    (s["ship_item_id"], s["name"], s["model"], s["price"])
                    for s in parsers.parse_ship_items(data_dir / "ShipItemInfo.txt")
                ),
            )
        if (data_dir / "character_lvup.txt").is_file():
            counts["level_costs"] = _insert_many(
                conn,
                "INSERT INTO level_costs VALUES (?,?,?)",
                _parse_level_costs(data_dir / "character_lvup.txt"),
            )

        conn.executemany(
            "INSERT INTO meta VALUES (?,?)",
            [
                ("source_dir", str(data_dir)),
                ("item_count", str(counts.get("items", 0))),
                ("monster_count", str(counts.get("monsters", 0))),
                ("npc_count", str(counts.get("npcs", 0))),
                ("spawn_count", str(counts.get("monster_spawns", 0))),
                ("shop_count", str(counts.get("shop_items", 0))),
                ("creature_stat_count", str(counts.get("creature_stats", 0))),
                ("drop_count", str(counts.get("creature_drops", 0))),
                ("dungeon_count", str(counts.get("dungeons", 0))),
                ("built_at", datetime.now().astimezone().isoformat(timespec="seconds")),
            ],
        )
        conn.commit()
    finally:
        conn.close()

    return counts


def main(argv: list[str] | None = None) -> int:
    argv = sys.argv[1:] if argv is None else argv
    data_dir = resolve_data_dir(argv[0] if argv else None)

    from ..config import settings

    db_path = settings.db_path
    counts = build(data_dir, db_path)

    print(f"Источник : {data_dir}")
    print(f"База     : {db_path}")
    for name, value in counts.items():
        print(f"  {name:<12} {value:>7}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())