"""Парсеры игровых таблиц ``.txt``.

Таблицы отдаются движком ``CRawDataSet``: таб-разделённые строки,
строка заголовка начинается с ``//``, кодировка варьируется
(UTF-8 / GBK / CP1251) в зависимости от того, какая таблица
попала в репозиторий.

Парсеры намеренно возвращают «сырые» значения: никакой доменной
логики, только разделение строк и приведение типов. Всё остальное
живёт в :mod:`app.etl.ingest`.
"""

from __future__ import annotations

import io
from pathlib import Path
from typing import Iterator, Sequence

from . import cha_columns as cha
from . import item_columns as col

# Кодировки, в которых в этом проекте встречаются таблицы.
# Порядок важен: utf-8 проверяется первым, т.к. он строже всех.
ENCODINGS = ("utf-8", "gbk", "cp1251")


def read_table(path: Path) -> list[list[str]]:
    """Читает таб-разделённую таблицу и возвращает список строк.

    Пустые строки и строки-заголовки (``//...``) отбрасываются.
    Кодировка определяется автоматически, последний вариант —
    ``latin-1``, который не падает ни на одном байте.
    """
    raw = path.read_bytes()
    text = None
    for enc in (*ENCODINGS, "latin-1"):
        try:
            text = raw.decode(enc)
            break
        except UnicodeDecodeError:
            continue
    assert text is not None  # latin-1 всегда декодирует

    rows: list[list[str]] = []
    for line in io.StringIO(text).read().splitlines():
        line = line.strip()
        if not line or line.startswith("//"):
            continue
        rows.append(line.split("\t"))
    return rows


def _cell(row: Sequence[str], index: int, default: str = "") -> str:
    """Безопасно достаёт ячейку: в таблицах встречаются короткие строки."""
    return row[index].strip() if index < len(row) else default


def to_int(value: str, default: int = 0) -> int:
    """Мягкое приведение к int: ``'-1'``, ``'0,0'``, ``''`` -> default."""
    try:
        return int(value.strip())
    except (TypeError, ValueError):
        return default


def to_pair(value: str) -> tuple[int, int]:
    """Разбирает ``'2092,2654'`` -> ``(2092, 2654)``."""
    parts = value.split(",")
    if len(parts) != 2:
        return (0, 0)
    return (to_int(parts[0]), to_int(parts[1]))


def to_int_list(value: str) -> list[int]:
    """Разбирает ``'1,8,9,10'`` -> ``[1, 8, 9, 10]``."""
    if not value:
        return []
    return [to_int(p) for p in value.split(",") if p.strip() != ""]


# ``itemtype.txt`` частично остался на китайском — переводим эти коды,
# иначе в каталоге предметов половина типов будет иероглифами.
ITEM_TYPE_RU = {
    31: "Восстановление",
    32: "Усиление",
    33: "Спецэффект",
    41: "Обычный предмет",
    51: "Нос корабля",
    52: "Корпус корабля",
    54: "Пушка корабля",
    55: "Винт корабля",
    56: "Флаг корабля",
    61: "Основание пушки",
    63: "Ствол пушки",
    64: "Флаг пушки",
}


def parse_itemtype(path: Path) -> dict[int, str]:
    """``itemtype.txt``: код типа -> название типа предмета."""
    out: dict[int, str] = {}
    for row in read_table(path):
        code = to_int(_cell(row, 0))
        name = _cell(row, 1)
        out[code] = ITEM_TYPE_RU.get(code, name)
    return out


def parse_item_prefix(path: Path) -> dict[int, str]:
    """``itempre.txt``: код префикса -> название (Mammoth, Colossus, ...)."""
    out: dict[int, str] = {}
    for row in read_table(path):
        out[to_int(_cell(row, 0))] = _cell(row, 1)
    return out


def parse_items(path: Path) -> Iterator[dict]:
    """``ItemInfo.txt``: 6337 предметов.

    Карта колонок восстановлена из ``CItemRecordSet::_ReadRawDataInfo``
    и описана в :mod:`app.etl.item_columns`. Ключевые поля:

    ============  =================================================
    Колонка       Значение
    ============  =================================================
    0             ID предмета
    1             Название
    2             Иконка (имя ресурса, ``w0001``)
    10            Тип
    16..19        Торгуется / подбирается / выбрасывается / удаляется
    20            Размер стака
    22            Цена
    24            Требуемый уровень
    25            Профессии
    28            Слоты экипировки
    31..51        Коэффициенты бонусов
    52..72        Постоянные бонусы «мин,макс»
    93            Текст описания
    ============  =================================================
    """
    for row in read_table(path):
        bonuses: list[dict] = []
        for i, field in enumerate(col.BONUS_FIELDS):
            low, high = to_pair(_cell(row, col.VALU_START + i))
            if not low and not high:
                continue
            bonuses.append(
                {
                    "field": field,
                    "title": col.BONUS_RU.get(field, field),
                    "min": low,
                    "max": high,
                }
            )

        description = _cell(row, col.C_DESCRIPTOR)
        if description == "0":
            description = ""

        endure_low, endure_high = to_pair(_cell(row, col.C_ENDURANCE))

        yield {
            "item_id": to_int(_cell(row, col.C_ID)),
            "name": _cell(row, col.C_NAME),
            "icon": _cell(row, col.C_ICON),
            "type_id": to_int(_cell(row, col.C_TYPE)),
            "forge_level": to_int(_cell(row, col.C_FORGE_LEVEL)),
            "forge_steady": to_int(_cell(row, col.C_FORGE_STEADY)),
            "set_id": to_int(_cell(row, col.C_SET_ID)),
            "prefix_chance": to_int(_cell(row, col.C_PREFIX_CHANCE)),
            "price": to_int(_cell(row, col.C_PRICE)),
            "pile_max": to_int(_cell(row, col.C_PILE_MAX)),
            "required_level": to_int(_cell(row, col.C_NEED_LEVEL)),
            "jobs": to_int_list(_cell(row, col.C_JOB)),
            "equip_slots": to_int_list(_cell(row, col.C_ABLE_LINK)),
            "body_sizes": to_int_list(_cell(row, col.C_BODY_SIZE)),
            "is_trade": to_int(_cell(row, col.C_IS_TRADE)) == 1,
            "is_pickup": to_int(_cell(row, col.C_IS_PICKUP)) == 1,
            "is_throw": to_int(_cell(row, col.C_IS_THROW)) == 1,
            "is_delete": to_int(_cell(row, col.C_IS_DELETE)) == 1,
            "instantiated": to_int(_cell(row, col.C_INSTANTIATED)) == 1,
            "exclusive": to_int(_cell(row, col.C_EXCLUSIVE_ID)) != 0,
            "endurance": endure_high or endure_low,
            "holes": to_int(_cell(row, col.C_HOLES)),
            "use_effect": _cell(row, col.C_USE_EFFECT),
            "description": description,
            "bonuses": bonuses,
        }


def parse_monsters(path: Path) -> Iterator[dict]:
    """``monsterlist.txt``: ID, имя, уровень, координаты, регион.

    Заголовок в файле устаревший и врёт про колонки, ориентируемся
    на фактические данные: ``1  Mystic Shrub  1  2092,2654  Ascaron``.

    Повторно описанная строка ``0  Monster Name ...`` отбрасывается.
    """
    for row in read_table(path):
        monster_id = to_int(_cell(row, 0))
        if monster_id <= 0:
            continue
        x, y = to_pair(_cell(row, 3))
        yield {
            "monster_id": monster_id,
            "name": _cell(row, 1),
            "level": to_int(_cell(row, 2)),
            "x": x,
            "y": y,
            "region": _cell(row, 4),
        }


def parse_npcs(path: Path) -> Iterator[dict]:
    """``npclist.txt``: ID, имя, место, координаты, регион.

    Фактический порядок колонок: ``1 Magician Chiatan Argent City 2229,2782 Ascaron``.
    """
    for row in read_table(path):
        x, y = to_pair(_cell(row, 3))
        yield {
            "npc_id": to_int(_cell(row, 0)),
            "name": _cell(row, 1),
            "location": _cell(row, 2),
            "x": x,
            "y": y,
            "region": _cell(row, 4),
        }


def parse_areas(path: Path) -> Iterator[dict]:
    """``AreaSet.txt``: ID, название, RGB карты, признак города."""
    for row in read_table(path):
        rgb = to_int_list(_cell(row, 2))
        yield {
            "area_id": to_int(_cell(row, 0)),
            "name": _cell(row, 1),
            "r": rgb[0] if len(rgb) > 0 else 0,
            "g": rgb[1] if len(rgb) > 1 else 0,
            "b": rgb[2] if len(rgb) > 2 else 0,
            "is_city": to_int(_cell(row, 7)) == 1,
        }


def parse_maps(path: Path) -> Iterator[dict]:
    """``mapinfo.txt``: ID, файл карты, название, координаты."""
    for row in read_table(path):
        x, y = to_pair(_cell(row, 4))
        yield {
            "map_id": to_int(_cell(row, 0)),
            "file": _cell(row, 1),
            "name": _cell(row, 2),
            "x": x,
            "y": y,
        }


def parse_creatures(path: Path) -> Iterator[dict]:
    """``CharacterInfo.txt``: реестр всех существ с настоящими ID.

    Именно эти ID используются в ``CreateCha`` из спавн-конфигов.
    ``monsterlist.txt`` использует другую нумерацию, поэтому имя
    и номер персонажа берутся именно отсюда.

    Колонки по заголовку файла: ID, Name, Short Name, Model Type,
    Logic Type (1/2 — классы игрока, 5 — монстры, прочие — NPC и
    служебные существа).
    """
    for row in read_table(path):
        creature_id = to_int(_cell(row, 0))
        if creature_id <= 0:
            continue
        yield {
            "creature_id": creature_id,
            "name": _cell(row, 1),
            "short_name": _cell(row, 2),
            "model_type": to_int(_cell(row, 3)),
            "logic_type": to_int(_cell(row, 4)),
        }


def _drop_pairs(row: Sequence[str]) -> Iterator[tuple[int, int, int]]:
    """Пары дропа из ячеек 46 и 47: ``(слот, ID предмета, частота)``.

    В ячейках значения перечислены через запятую, причём ведущие нули
    не проставлены (``0467``), а длины списков могут не совпадать —
    берём минимум и молча отбрасываем остаток.

    Частота хранится «обратной»: 10000 означает 100 %. На сайте
    показываем сырое число, поэтому пересчёт в проценты живёт
    только в интерфейсе и намеренно не выполняется тут.
    """
    items = to_int_list(_cell(row, cha.C_DROP_ITEMS))
    rates = to_int_list(_cell(row, cha.C_DROP_RATES))
    for slot, (item_id, rate) in enumerate(zip(items, rates)):
        if item_id <= 0:
            continue
        yield slot, item_id, rate


def parse_creature_drops(path: Path) -> Iterator[tuple[int, int, int, int]]:
    """``CharacterInfo.txt`` -> ``(ID существа, слот, ID предмета, частота)``."""
    for row in read_table(path):
        creature_id = to_int(_cell(row, cha.C_ID))
        if creature_id <= 0:
            continue
        for slot, item_id, rate in _drop_pairs(row):
            if slot >= cha.DROP_SLOTS:
                break
            yield creature_id, slot, item_id, rate


def parse_creature_stats(path: Path) -> Iterator[tuple]:
    """``CharacterInfo.txt`` -> боевая строка существа.

    Порядок полей задан :data:`cha_columns.STAT_FIELDS` и совпадает
    с порядком колонок в таблице ``creature_stats``. Только монстры:
    у игроков и NPC эти поля не заполнены, и в базе они не нужны.
    """
    for row in read_table(path):
        creature_id = to_int(_cell(row, cha.C_ID))
        if creature_id <= 0:
            continue
        if to_int(_cell(row, cha.C_LOGIC_TYPE)) != cha.LOGIC_MONSTER:
            continue
        yield (creature_id,) + tuple(
            to_int(_cell(row, column)) for _field, column in cha.STAT_FIELDS
        )


def parse_ship_items(path: Path) -> Iterator[dict]:
    """``ShipItemInfo.txt``: корпуса кораблей. Цена в колонке 7."""
    for row in read_table(path):
        yield {
            "ship_item_id": to_int(_cell(row, 0)),
            "name": _cell(row, 1),
            "model": to_int(_cell(row, 2)),
            "price": to_int(_cell(row, 7)),
        }