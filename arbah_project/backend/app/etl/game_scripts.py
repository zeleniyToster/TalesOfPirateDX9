"""Разбор игровых Lua-конфигураций: спавны монстров и ассортимент NPC.

Оба формата читаются напрямую из серверных скриптов — отдельных
таблиц для них в проекте нет.

Спавны: в каждом каталоге карты лежит ``<карта>monster_conf.lua``
с вызовами вида ``CreateCha(777, 209900, 275200, 145, 60)`` —
ID персонажа, X, Y, угол и время респауна в секундах.

Магазины: в ``MisScript/NpcScript*.lua`` функция ``r_talk73``
принадлежит NPC №73, а список товаров перечисляет вызовами
``Weapon(0008)``, ``Defence(0290)``, ``Other(...)``.
"""

from __future__ import annotations

import re
from pathlib import Path
from typing import Iterator

# CreateCha(ид, x, y, угол, респаун)
CREATE_CHA = re.compile(
    r"CreateCha\s*\(\s*(\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(\d+)\s*\)",
    re.IGNORECASE,
)
SET_CUR_MAP = re.compile(r'SetCurMap\s*\(\s*"([^"]+)"\s*\)', re.IGNORECASE)

FUNCTION = re.compile(r"^\s*function\s+(\w+)\s*\(", re.M)
INIT_TRADE = "InitTrade()"

# Номер NPC закодирован в имени функции — но это НЕ идентификатор NPC
# из npclist.txt. Например, r_talk235 — это Pucan, а в npclist он ID 70.
# Связь устанавливается только по имени из реплики Talk(), поэтому
# ниже номер функции используется лишь для группировки.
SHOP_NPC_ID = re.compile(r"^(?:r_talk|BT_NpcSale)(\d+)")

# Вызовы, добавляющие товар в список торговли. AddNpcMission,
# MisListPage, Talk и Text сюда не входят — это другое.
SHOP_ITEM_CALLS = ("Defence", "Weapon", "Other")
SHOP_ITEM = re.compile(
    r"^\s*(?:" + "|".join(SHOP_ITEM_CALLS) + r")\s*\(\s*0*(\d+)\s*\)", re.M
)
TALK_TEXT = re.compile(r'Talk\s*\(\s*\d+\s*,\s*"([^"]*)"')

# Расписание входа в данж: в каждом каталоге карты есть функция
# can_open_entry_<карта>. Если часов нет — вход свободный.
DUNGEON_FUNC = re.compile(r"function\s+can_open_entry_(\w+)\s*\([^)]*\)")
DUNGEON_HOUR = re.compile(r"\bHour\s*==\s*(\d{1,2})")


def build_npc_name_index(npcs: dict[int, str]) -> dict[str, int]:
    """Строит индекс «имя из реплики -> ID NPC».

    В ``npclist.txt`` имена оформлены как ``<роль> - <имя>``
    (``Accessory - Pucan``, ``Blacksmith - Bash``), а в репликах
    остаётся только ``Pucan``. Поэтому сравниваем последнюю часть
    после ``" - "``.

    Строка ``Dada`` не должна совпадать с ``Holy Priestess - Ada``,
    поэтому сравнение идёт с обоих концов, а не через ``endswith``.
    """
    index: dict[str, int] = {}
    for npc_id, full_name in npcs.items():
        short = full_name.split(" - ")[-1].strip().lower()
        if short and short not in index:
            index[short] = npc_id
        low = full_name.strip().lower()
        if low and low not in index:
            index[low] = npc_id
    return index


def _talk_name(body: str) -> str | None:
    """Имя NPC из первой реплики вида ``"Pucan: I have..."``."""
    for text in TALK_TEXT.findall(body):
        if ":" not in text:
            continue
        candidate = text.split(":", 1)[0].strip()
        if candidate and len(candidate) < 40:
            return candidate
    return None


def _read_lua(path: Path) -> str:
    raw = path.read_bytes()
    for enc in ("utf-8", "gbk", "latin-1"):
        try:
            return raw.decode(enc)
        except UnicodeDecodeError:
            continue
    return ""


def find_spawns_dir(resource_dir: Path) -> Path | None:
    """Где лежат конфиги спавнов.

    Поддерживаются две раскладки: плоская ``<resources>/spawns/*.lua``
    (так собран каталог внутри проекта) и штатная серверная
    ``<resources>/<карта>/<карта>monster_conf.lua``. Вторая нужна,
    когда серверная папка смонтирована в контейнер.
    """
    if any((resource_dir / "spawns").glob("*.lua")):
        return resource_dir / "spawns"
    if any(resource_dir.glob("*/*monster_conf.lua")):
        return resource_dir
    return None


def find_npc_script_dir(resource_dir: Path) -> Path | None:
    """Где лежат ``NpcScript*.lua``: ``<resources>/npc`` в проекте
    либо ``<resources>/script/MisScript`` в поставке сервера."""
    for candidate in (resource_dir / "npc", resource_dir / "script" / "MisScript"):
        if candidate.is_dir() and any(candidate.glob("NpcScript*.lua")):
            return candidate
    return None


def parse_spawns(resource_dir: Path) -> Iterator[dict]:
    """Все ``CreateCha`` со всех карт.

    Принимает каталог с ресурсами, а не каталог со спавнами: путь
    ищется сам, см. :func:`find_spawns_dir`. Регион берётся из
    ``mapinfo.txt`` того же каталога. Имя карты — из суффикса
    ``<карта>monster_conf.lua``.
    """
    spawns_dir = find_spawns_dir(resource_dir)
    if spawns_dir is None:
        return

    regions = _load_regions(resource_dir)

    confs = sorted(spawns_dir.glob("*.lua")) + sorted(
        spawns_dir.glob("*/*monster_conf.lua")
    )

    for conf in confs:
        text = _read_lua(conf)
        if not text:
            continue

        stem = conf.name
        suffix = "monster_conf.lua"
        map_file = stem[: -len(suffix)] if stem.endswith(suffix) else conf.parent.name
        region = regions.get(map_file, map_file)

        for match in CREATE_CHA.finditer(text):
            yield (
                int(match.group(1)),
                map_file,
                region,
                int(match.group(2)),
                int(match.group(3)),
                int(match.group(4)),
                int(match.group(5)),
            )


def _load_regions(resources_dir: Path) -> dict[str, str]:
    """``mapinfo.txt``: имя файла карты -> название региона."""
    out: dict[str, str] = {}
    path = resources_dir / "mapinfo.txt"
    if not path.is_file():
        return out
    for line in io_lines(path):
        cols = line.split("\t")
        if len(cols) >= 3:
            out[cols[1].strip()] = cols[2].strip()
    return out


def io_lines(path: Path) -> list[str]:
    """Строки файла без заголовков и комментариев, в любой кодировке."""
    raw = path.read_bytes()
    for enc in ("utf-8", "gbk", "latin-1"):
        try:
            text = raw.decode(enc)
            break
        except UnicodeDecodeError:
            continue
    else:
        return []
    return [
        line.strip()
        for line in text.splitlines()
        if line.strip() and not line.strip().startswith("//")
    ]


def parse_dungeon_schedules(resource_dir: Path) -> Iterator[dict]:
    """Расписание входа в данжи из ``<карта>/ctrl.lua``.

    Функция ``can_open_entry_<карта>`` сравнивает текущий час с
    перечислением: ``if Hour == 02 or Hour == 06 then``. Если
    сравнений нет, вход свободен в любое время.

    Время берётся из ``GetNowTime()`` — это серверное локальное
    время (``os.date`` без ``!Z``), то есть часовой пояс машины.
    Отдельно от расписания спавня боссов в ``DreamIsland`` ничего
    не разбираем: там другой механизм, на ``can_open_entry`` он не
    влияет.

    Отдаёт ``{'map_file', 'always_open', 'hours'}``; ``hours`` —
    строка ``"2,6,10,14,18,22"``, как принято для прочих списков.
    """
    if not resource_dir.is_dir():
        return

    for ctrl in sorted(resource_dir.glob("*/ctrl.lua")):
        text = _read_lua(ctrl)
        if not text:
            continue

        match = DUNGEON_FUNC.search(text)
        if not match:
            continue

        nxt = text.find("\nfunction", match.start() + 1)
        body = text[match.start(): nxt if nxt > 0 else len(text)]

        hours = sorted({int(h) for h in DUNGEON_HOUR.findall(body)})
        yield {
            "map_file": ctrl.parent.name,
            "always_open": not hours,
            "hours": ",".join(str(h) for h in hours),
        }


def parse_shops(
    npc_script_dir: Path, npc_names: dict[int, str]
) -> Iterator[tuple[int, int]]:
    """Ассортимент NPC из ``NpcScript*.lua``.

    Возвращает пары (ID NPC, ID предмета). NPC определяется по имени
    из реплики, а не по номеру функции: см. :func:`build_npc_name_index`.
    """
    index = build_npc_name_index(npc_names)

    for path in sorted(npc_script_dir.glob("NpcScript*.lua")):
        text = _read_lua(path)
        if not text:
            continue

        for _npc_id, items, _name in _iter_trades(text, index):
            for item_id in items:
                yield _npc_id, item_id


def shop_dialogue(
    npc_script_dir: Path, npc_names: dict[int, str]
) -> dict[int, str]:
    """Первая реплика торговца — пригодится для описания на карточке."""
    index = build_npc_name_index(npc_names)
    out: dict[int, str] = {}
    for path in sorted(npc_script_dir.glob("NpcScript*.lua")):
        for npc_id, _items, text in _iter_trades(_read_lua(path), index):
            if text and npc_id not in out:
                out[npc_id] = text
    return out


def _iter_trades(text: str, index: dict[str, int]):
    """Отдаёт (ID NPC, список товаров, реплика) для каждого торговца."""
    if not text:
        return
    funcs = [(m.group(1), m.start()) for m in FUNCTION.finditer(text)]

    for i, (name, start) in enumerate(funcs):
        end = funcs[i + 1][1] if i + 1 < len(funcs) else len(text)
        body = text[start:end]

        if INIT_TRADE not in body:
            continue

        items = sorted({int(x) for x in SHOP_ITEM.findall(body)})
        if not items:
            continue

        talks = TALK_TEXT.findall(body)
        npc_name = _talk_name(body)
        npc_id = index.get(npc_name.lower()) if npc_name else None
        if npc_id is None:
            continue

        yield npc_id, items, (talks[0] if talks else "")