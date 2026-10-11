"""Тесты разбора дропа, боевых характеристик и расписания данжей.

Парсерные проверки идут на синтетических файлах, чтобы не зависеть
от игровых таблиц: проверяется именно раскладка колонок и логика
отбраковки. Последний тест дополнительно сверяет базу с эталоном
из панели Mob Info клиента.
"""

from __future__ import annotations

import sqlite3
from dataclasses import replace
from pathlib import Path

import pytest
from fastapi.testclient import TestClient

from app.etl import game_scripts, parsers
from app.etl.cha_columns import C_DROP_ITEMS, C_DROP_RATES, C_LEVEL, C_MAX_HP
from app.main import app

DATA_DIR = Path(__file__).resolve().parents[2] / "data"
DB_PATH = DATA_DIR / "arbah.db"

# Значения из окна Mob Info для Killer Shroom (ID 252).
KILLER_SHROOM_STATS = {
    "level": 19,
    "max_hp": 457,
    "max_sp": 10000,
    "min_atk": 63,
    "max_atk": 94,
    "phys_res": 9,
    "defense": 43,
    "hit": 61,
    "dodge": 61,
    "atk_speed": 2000,
    "move_speed": 300,
    "exp": 226,
}


def _cha_row(**overrides: str) -> str:
    """Строка ``CharacterInfo.txt`` нужной длины с заданными ячейками."""
    cells = [""] * 95
    cells[0] = overrides.get("id", "1")
    cells[1] = overrides.get("name", "Test Monster")
    cells[2] = "Test"
    cells[3] = "5"
    cells[4] = overrides.get("logic", "5")
    for key, value in overrides.items():
        if key in ("id", "name", "logic"):
            continue
        if isinstance(value, tuple):
            index, text = value
            cells[index] = text
    return "\t".join(cells)


def _write_cha(tmp_path: Path, *rows: str) -> Path:
    path = tmp_path / "CharacterInfo.txt"
    path.write_text("// заголовок\n" + "\n".join(rows) + "\n", encoding="utf-8")
    return path


# ------------------------------------------------------------------ дроп
def test_drops_are_split_into_item_and_rate(tmp_path: Path) -> None:
    path = _write_cha(
        tmp_path,
        _cha_row(
            id="7",
            items=(C_DROP_ITEMS, "4345,4429,1655"),
            rates=(C_DROP_RATES, "196,294,1428"),
        ),
    )
    rows = list(parsers.parse_creature_drops(path))
    assert rows == [
        (7, 0, 4345, 196),
        (7, 1, 4429, 294),
        (7, 2, 1655, 1428),
    ]


def test_leading_zeros_in_item_id_are_tolerated(tmp_path: Path) -> None:
    path = _write_cha(
        tmp_path,
        _cha_row(
            id="1", items=(C_DROP_ITEMS, "0467,0643"), rates=(C_DROP_RATES, "100000,200000")
        ),
    )
    assert [r[2] for r in parsers.parse_creature_drops(path)] == [467, 643]


def test_uneven_lists_stop_at_shortest(tmp_path: Path) -> None:
    """В реальной таблице списки могут разной длины — лишнее молча
    отбрасывается, чтобы не выехать за пределы массива."""
    path = _write_cha(
        tmp_path,
        _cha_row(id="1", items=(C_DROP_ITEMS, "10,20,30"), rates=(C_DROP_RATES, "100,200")),
    )
    assert len(list(parsers.parse_creature_drops(path))) == 2


def test_zero_item_ids_are_skipped(tmp_path: Path) -> None:
    path = _write_cha(
        tmp_path,
        _cha_row(id="1", items=(C_DROP_ITEMS, "0,55"), rates=(C_DROP_RATES, "0,100")),
    )
    assert [r[2] for r in parsers.parse_creature_drops(path)] == [55]


def test_creature_without_drops_yields_nothing(tmp_path: Path) -> None:
    path = _write_cha(tmp_path, _cha_row(id="3"))
    assert list(parsers.parse_creature_drops(path)) == []


# ------------------------------------------------------------- статы
def test_stats_are_read_from_verified_columns(tmp_path: Path) -> None:
    path = _write_cha(
        tmp_path,
        _cha_row(
            id="252",
            level=(C_LEVEL, "19"),
            hp=(C_MAX_HP, "457"),
        ),
    )
    (creature_id, level, max_hp) = list(parsers.parse_creature_stats(path))[0][:3]
    assert (creature_id, level, max_hp) == (252, 19, 457)


def test_stats_skip_non_monsters(tmp_path: Path) -> None:
    """У классов игроков (логика 1 и 2) боевые поля не заполнены,
    в базу они попадать не должны."""
    path = _write_cha(
        tmp_path,
        _cha_row(id="1", logic="1"),
        _cha_row(id="2", logic="2"),
        _cha_row(id="3", logic="5"),
    )
    assert [r[0] for r in parsers.parse_creature_stats(path)] == [3]


# --------------------------------------------------------- расписание данжей
def _write_ctrl(tmp_path: Path, folder: str, body: str) -> Path:
    directory = tmp_path / folder
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "ctrl.lua").write_text(
        "function OnInit(map)\nend\n\n" + body + "\n", encoding="utf-8"
    )
    return tmp_path


HOURS_BODY = """function can_open_entry_abandonedcity(map)
\tlocal Hour = GetNowTime()
\tif Hour == 02 or Hour == 06 or Hour == 10 then
\t\treturn 1
\tend
\treturn 0
end"""

ALWAYS_BODY = """function can_open_entry_hell(map)
\treturn 1
end"""


def test_dungeon_hours_are_collected(tmp_path: Path) -> None:
    root = _write_ctrl(tmp_path, "abandonedcity", HOURS_BODY)
    assert list(game_scripts.parse_dungeon_schedules(root)) == [
        {"map_file": "abandonedcity", "always_open": False, "hours": "2,6,10"}
    ]


def test_dungeon_without_hours_is_always_open(tmp_path: Path) -> None:
    """``return 1`` внутри ``if`` не делает данж открытым всегда —
    признак отсутствия ограничения в том, что нет сравнений с часом."""
    root = _write_ctrl(tmp_path, "hell", ALWAYS_BODY)
    assert list(game_scripts.parse_dungeon_schedules(root)) == [
        {"map_file": "hell", "always_open": True, "hours": ""}
    ]


def test_maps_without_can_open_entry_are_skipped(tmp_path: Path) -> None:
    root = _write_ctrl(tmp_path, "garner", "function OnInit(map)\nend")
    assert list(game_scripts.parse_dungeon_schedules(root)) == []


def test_missing_resource_dir_is_not_an_error(tmp_path: Path) -> None:
    assert list(game_scripts.parse_dungeon_schedules(tmp_path / "нет")) == []


# ------------------------------------------------ сверка с эталоном клиента
@pytest.fixture(scope="module")
def db() -> sqlite3.Connection:
    if not DB_PATH.is_file():
        pytest.skip("база не собрана: python -m app.etl.ingest")
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def test_database_stats_match_client_panel(db: sqlite3.Connection) -> None:
    row = db.execute(
        "SELECT * FROM creature_stats WHERE creature_id = 252"
    ).fetchone()
    assert row is not None, "Killer Shroom не найден в creature_stats"
    for key, expected in KILLER_SHROOM_STATS.items():
        assert row[key] == expected, f"{key}: {row[key]} != {expected}"


def test_database_drops_resolve_to_items(db: sqlite3.Connection) -> None:
    """Каждый предмет дропа должен существовать в items, иначе в
    карточке монстра окажутся пустые строки."""
    rows = db.execute(
        """SELECT d.item_id, d.rate, i.name
           FROM creature_drops d LEFT JOIN items i ON i.item_id = d.item_id
           WHERE d.creature_id = 252 ORDER BY d.slot"""
    ).fetchall()
    assert len(rows) == 7
    for row in rows:
        assert row["name"], f"предмет {row['item_id']} не найден в items"
        assert row["rate"] > 0


def test_database_dungeons_loaded(db: sqlite3.Connection) -> None:
    total = db.execute("SELECT COUNT(*) FROM dungeons").fetchone()[0]
    scheduled = db.execute(
        "SELECT COUNT(*) FROM dungeons WHERE always_open = 0"
    ).fetchone()[0]
    assert total > 0, "данные о расписании не загружены"
    assert scheduled > 0


# ------------------------------------------------------------------- API
@pytest.fixture(scope="module")
def client() -> TestClient:
    return TestClient(app)


def test_creature_card_exposes_stats(client: TestClient) -> None:
    body = client.get("/api/catalog/creatures/252/full").json()
    stats = body["stats"]
    assert stats is not None
    for key, expected in KILLER_SHROOM_STATS.items():
        assert stats[key] == expected, f"{key}: {stats[key]} != {expected}"
        assert stats[f"{key}_title"], f"нет подписи для {key}"


def test_creature_card_exposes_drops_with_raw_rate(client: TestClient) -> None:
    """Частота отдаётся сырой: пересчитывать её в проценты на сервере
    нельзя, потому что отображаемое значение зависит от рейта."""
    drops = client.get("/api/catalog/creatures/252/full").json()["drops"]
    assert len(drops) == 7
    assert [d["slot"] for d in drops] == list(range(7))
    assert drops[0]["item_id"] == 4345
    assert drops[0]["rate"] == 196
    for d in drops:
        assert d["rate"] > 0
        assert d["name"], f"предмет {d['item_id']} не разрешился"


def test_creature_without_stats_returns_null(client: TestClient) -> None:
    """Существо-не-монстр (например, класс игрока) не должно падать:
    характеристик у него нет, возвращаем null."""
    body = client.get("/api/catalog/creatures/1/full").json()
    assert "stats" in body


def test_dungeons_endpoint_lists_schedule(client: TestClient) -> None:
    body = client.get("/api/catalog/dungeons").json()
    assert body["total"] > 0
    by_map = {d["map_file"]: d for d in body["dungeons"]}

    assert by_map["abandonedcity"]["hours"] == [2, 6, 10, 14, 18, 22]
    assert by_map["abandonedcity"]["always_open"] is False
    assert by_map["hell"]["hours"] == [15]
    assert by_map["binglang2"]["hours"] == [13, 19]

    for d in body["dungeons"]:
        assert d["always_open"] or d["hours"], f"{d['map_file']}: пустое расписание"


def test_dungeons_open_now_follows_hour(client: TestClient) -> None:
    """Отметка «открыт сейчас» обязана согласовываться с часом
    сервера, который эндпоинт возвращает рядом."""
    body = client.get("/api/catalog/dungeons").json()
    hour = body["hour"]
    for d in body["dungeons"]:
        if d["always_open"]:
            assert d["open_now"] is True
        else:
            assert d["open_now"] == (hour in d["hours"]), d["map_file"]


def test_dungeons_resolve_map_names(client: TestClient) -> None:
    """Имя карты подтягивается из mapinfo.txt: без него в интерфейсе
    остались бы английские идентификаторы папок."""
    body = client.get("/api/catalog/dungeons").json()
    by_map = {d["map_file"]: d for d in body["dungeons"]}
    assert by_map["abandonedcity"]["name"] != "abandonedcity"


# -------------------------------------------------- обслуживание базы
@pytest.fixture
def isolated_db(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    """Подменяет путь к базе на временный.

    Пересборка перезаписывает файл целиком, поэтому тесты обслуживания
    не должны трогать общую базу: собранную из плоского каталога
    ``arbah_project/data``, она лишилась бы расписания данжей, для
    которого нужны подкаталоги карт.
    """
    from app.config import settings as real_settings
    from app.routers import admin

    fake = replace(real_settings, db_path=tmp_path / "arbah.db")
    monkeypatch.setattr(admin, "settings", fake)
    return fake


def test_refresh_status_works_before_first_build(client: TestClient) -> None:
    """Статус должен отвечать и когда файла базы ещё нет: иначе
    сразу после старта контейнера он недоступен."""
    body = client.get("/api/admin/refresh/status").json()
    assert "data_dir" in body
    assert "in_progress" in body


def test_refresh_rebuilds_from_data_dir(
    client: TestClient, isolated_db: Path
) -> None:
    """Пересборка обязана читать игровые таблицы заново и возвращать
    счётчики — это и есть механизм синхронизации после правки
    дропа или расписания данжей."""
    assert not isolated_db.db_path.exists()

    payload = client.post("/api/admin/refresh").json()
    assert payload["status"] == "ok"
    assert payload["seconds"] >= 0
    assert payload["counts"]["items"] > 6000
    assert payload["counts"]["creature_drops"] > 0
    assert isolated_db.db_path.is_file()


def test_refresh_records_build_time(client: TestClient, isolated_db: Path) -> None:
    client.post("/api/admin/refresh")
    status = client.get("/api/admin/refresh/status").json()
    assert status["database_ready"] is True
    assert status["updated_at"], "в meta нет отметки времени сборки"
    assert status["in_progress"] is False