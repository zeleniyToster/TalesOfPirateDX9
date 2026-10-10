"""Тесты API.

Запуск::

    $env:PYTHONPATH="backend"
    .venv\\Scripts\\python.exe -m pytest backend/tests -q
"""

from __future__ import annotations

import pytest
from fastapi.testclient import TestClient

from app.main import app
from app.services.calculator import calculate, load_coefficients


@pytest.fixture(scope="module")
def client() -> TestClient:
    return TestClient(app)


# ------------------------------------------------------------------ каталог
def test_health(client: TestClient) -> None:
    body = client.get("/api/health").json()
    assert body["status"] == "ok"
    assert body["counts"]["items"] > 6000


def test_items_search_by_name(client: TestClient) -> None:
    body = client.get("/api/catalog/items", params={"q": "Phoenix"}).json()
    assert body["page"]["total"] > 0
    assert any("Phoenix" in it["name"] for it in body["items"])


def test_items_search_by_id(client: TestClient) -> None:
    body = client.get("/api/catalog/items", params={"q": "1092"}).json()
    assert any(it["item_id"] == 1092 for it in body["items"])


def test_items_filter_by_level(client: TestClient) -> None:
    body = client.get(
        "/api/catalog/items", params={"min_level": 60, "page_size": 100}
    ).json()
    assert all(it["required_level"] >= 60 for it in body["items"])


def test_item_by_id_404(client: TestClient) -> None:
    assert client.get("/api/catalog/items/999999999").status_code == 404


def test_sort_injection_is_rejected(client: TestClient) -> None:
    """Поле сортировки подставляется в SQL — проверяем белый список."""
    res = client.get("/api/catalog/items", params={"sort": "name; DROP TABLE items"})
    assert res.status_code == 400
    # таблица на месте
    assert client.get("/api/catalog/items").status_code == 200


def test_monsters_regions(client: TestClient) -> None:
    body = client.get("/api/catalog/monsters/regions").json()
    assert len(body) >= 3
    assert "Ascaron" in {r["name"] for r in body}


def test_npcs(client: TestClient) -> None:
    body = client.get("/api/catalog/npcs", params={"q": "Magician"}).json()
    assert body["page"]["total"] > 0


# ------------------------------------------------------------------- опыт
def test_level_costs_150(client: TestClient) -> None:
    rows = client.get("/api/levels/costs").json()
    assert len(rows) == 150
    assert rows[-1]["level"] == 150
    # накопленный опыт строго растёт
    totals = [r["exp_total"] for r in rows]
    assert totals == sorted(totals)
    assert len(set(totals)) == len(totals)


def test_exp_to_level(client: TestClient) -> None:
    body = client.get(
        "/api/levels/exp-to-level", params={"current_level": 60, "current_exp": 0}
    ).json()
    assert body["next_level"] == 61
    assert body["left"] == body["needed"] > 0


# -------------------------------------------------------------- калькулятор
def test_coefficients_loaded() -> None:
    data = load_coefficients()
    assert len(data["jobs"]) == 19
    assert "Mxhp_con_rad1" in data["tables"]


def test_stats_match_hand_calculation() -> None:
    """Формула проверена вручную по AttrCalculate.lua:1647.

    Мечник (JOB=1), уровень 1, CON=10:
      10*3*5 + 7*floor(30/20)^2 + 1*25 + 40 = 150 + 7 + 25 + 40 = 222
    """
    result = calculate(
        job=1, level=1,
        strength=10, dexterity=10, agility=10,
        constitution=10, stamina=10, luck=10,
    )
    assert result["max_hp"] == 222


def test_stats_scales_with_constitution() -> None:
    base = dict(job=1, level=50, strength=40, dexterity=40,
                agility=40, stamina=40, luck=10)
    low = calculate(constitution=30, **base)
    high = calculate(constitution=60, **base)
    assert high["max_hp"] > low["max_hp"]
    assert high["defence"] > low["defence"]


def test_stats_via_api(client: TestClient) -> None:
    res = client.post(
        "/api/calculator/stats",
        json={
            "job": 1, "level": 100,
            "strength": 60, "dexterity": 60, "agility": 60,
            "constitution": 60, "stamina": 60, "luck": 20,
        },
    )
    assert res.status_code == 200
    body = res.json()
    assert body["max_hp"] > 0
    assert body["min_attack"] > 0
    assert body["hit"] == 60 * body["hit"] // 60  # арифметика целая


def test_stats_rejects_bad_level(client: TestClient) -> None:
    res = client.post("/api/calculator/stats", json={"job": 1, "level": 999})
    assert res.status_code == 422


# ------------------------------------------------------------- карточки
def test_item_card_has_all_sections(client: TestClient) -> None:
    body = client.get("/api/catalog/items/43/full").json()
    assert body["name"] == "Firegun of Sol"
    # цена и уровень берутся из колонок 22 и 24 ItemInfo.txt
    assert body["price"] == 578932
    assert body["required_level"] == 80
    assert body["jobs"] == ["Стрелок"]
    # слот 9 в enumEQUIP_* — это правая рука, а не ожерелье
    assert body["equip_slots"] == ["Правая рука"]
    assert body["bonuses"], "у огнестрела должны быть бонусы"
    assert "trade" in body["flags"]
    assert body["gm_command"].startswith("make 43")


def test_item_card_steel_is_stackable(client: TestClient) -> None:
    """Короткий меч не стакается (pile_max = 1), посох — стакается."""
    sword = client.get("/api/catalog/items/1/full").json()
    assert sword["pile_max"] == 1
    assert sword["description"], "у меча должно быть описание из колонки 93"


def test_creature_card(client: TestClient) -> None:
    body = client.get("/api/catalog/creatures/721/full").json()
    assert body["name"] == "Wailing Marksman"
    assert body["spawn_count"] > 100
    assert body["areas"], "должны быть области обитания"
    assert body["respawns"], "должен быть интервал респауна"


def test_spawn_area_box(client: TestClient) -> None:
    """Рамка обитания = min/max координат, делённые на 100.

    Проверено против piratia.arbah.ru: у «Воображаемой снегурки»
    там указано 20/19 - 522/527, здесь должно совпасть до цифры.
    """
    body = client.get("/api/catalog/creatures/669/full").json()
    assert body["name"] == "Imaginary Snow Doll"
    assert body["spawn_count"] == 515

    area = body["areas"][0]
    assert area["region"] == "Demonic World"
    assert area["count"] == 515
    assert area["respawns"] == [120], "120 секунд — это 2 минуты"
    assert area["area"] == "20/19 - 522/527"


def test_area_box_is_min_max_of_points(client: TestClient) -> None:
    """Рамка обязана совпадать с реальными точками спавна."""
    import sqlite3

    from app.config import settings

    conn = sqlite3.connect(settings.db_path)
    rows = conn.execute(
        "SELECT x, y FROM monster_spawns WHERE char_id = 669"
    ).fetchall()
    conn.close()

    min_x = min(r[0] for r in rows) // 100
    max_x = max(r[0] for r in rows) // 100
    min_y = min(r[1] for r in rows) // 100
    max_y = max(r[1] for r in rows) // 100

    body = client.get("/api/catalog/creatures/669/full").json()
    assert body["areas"][0]["area"] == f"{min_x}/{min_y} - {max_x}/{max_y}"


def test_creature_search(client: TestClient) -> None:
    """Морские мобы (акулы и прочие) не имеют статических точек спавна —
    они создаются по другой логике, поэтому в выдаче по умолчанию
    их нет. Проверяем оба режима."""
    spawned = client.get(
        "/api/catalog/creatures/search", params={"q": "shark", "spawned_only": True}
    ).json()
    assert all(c["spawn_count"] > 0 for c in spawned)

    all_found = client.get(
        "/api/catalog/creatures/search", params={"q": "shark", "spawned_only": False}
    ).json()
    assert len(all_found) > len(spawned), "акулы должны находиться без фильтра"


def test_creature_search_finds_spawned(client: TestClient) -> None:
    body = client.get(
        "/api/catalog/creatures/search", params={"q": "marksman"}
    ).json()
    assert body
    assert body[0]["spawn_count"] > 0


def test_npc_shop(client: TestClient) -> None:
    # NPC 170 — портной, крупнейший ассортимент в игре
    body = client.get("/api/catalog/npcs/170/shop").json()
    assert body["npc"]["name"] == "Tailor - Moya"
    assert body["page"]["total"] == 84
    assert all("gm_command" in i for i in body["items"])


def test_npc_card_works_without_shop(client: TestClient) -> None:
    """Карточка должна открываться для всех 482 NPC, а не только
    для торговых: у остальных просто пустой ассортимент."""
    import sqlite3

    from app.config import settings

    conn = sqlite3.connect(settings.db_path)
    npc_id = conn.execute(
        "SELECT npc_id FROM npcs "
        "WHERE npc_id NOT IN (SELECT npc_id FROM shop_items) LIMIT 1"
    ).fetchone()[0]
    conn.close()

    res = client.get(f"/api/catalog/npcs/{npc_id}/shop")
    assert res.status_code == 200
    body = res.json()
    assert body["page"]["total"] == 0
    assert body["items"] == []
    assert body["npc"]["name"], "название должно быть даже без товаров"
    assert "npc_id" in body["npc"]


def test_npc_card_404_for_unknown(client: TestClient) -> None:
    assert client.get("/api/catalog/npcs/999999/shop").status_code == 404


def test_shop_is_bound_by_name_not_function_number(client: TestClient) -> None:
    """Регрессия: номер функции r_talkNN не совпадает с ID NPC.

    Pucan описан в функции r_talk235, а в npclist.txt он ID 70.
    Сопоставление по номеру функции отдавало ассортимент не тому NPC.
    """
    pucan = client.get("/api/catalog/npcs/70/shop").json()
    assert pucan["npc"]["name"] == "Accessory - Pucan"
    assert pucan["page"]["total"] == 10
    assert all("Ring" in i["name"] for i in pucan["items"])

    # Bash тоже живёт не под своим номером: r_talk73 -> ID 280
    bash = client.get("/api/catalog/npcs/280/shop").json()
    assert bash["npc"]["name"] == "Blacksmith - Bash"
    assert bash["page"]["total"] > 0


def test_priestess_has_no_shop(client: TestClient) -> None:
    """У жрицы Ады только квесты — торговли быть не должно."""
    ada = client.get("/api/catalog/npcs/209/shop").json()
    assert ada["npc"]["name"] == "Holy Priestess - Ada"
    assert ada["page"]["total"] == 0


def test_shop_only_filter(client: TestClient) -> None:
    all_npcs = client.get("/api/catalog/npcs", params={"page_size": 200}).json()
    traders = client.get(
        "/api/catalog/npcs", params={"shop_only": True, "page_size": 200}
    ).json()
    assert all_npcs["page"]["total"] > 400
    assert 0 < traders["page"]["total"] < all_npcs["page"]["total"]
    ids = {n["npc_id"] for n in traders["npcs"]}
    assert 70 in ids, "Pucan обязан попасть в список торговцев"


def test_shop_items_are_resolvable(client: TestClient) -> None:
    """Каждый товар в магазине должен существовать в каталоге предметов."""
    body = client.get("/api/catalog/npcs/170/shop", params={"page_size": 100}).json()
    assert body["page"]["total"] > 50
    for item in body["items"]:
        res = client.get(f"/api/catalog/items/{item['item_id']}/full")
        assert res.status_code == 200, item["item_id"]


def test_spawns_reference_known_creatures(client: TestClient) -> None:
    """Каждое CreateCha должно указывать на существо из реестра."""
    body = client.get("/api/health").json()
    assert body["counts"]["spawns"] > 5000
    assert body["counts"]["creatures"] > 1000


# ----------------------------------------------------------------- иконки
def test_icon_route_resolves_extension(client: TestClient) -> None:
    """Иконка отдаётся без знания расширения: в папке есть .png, .tga, .bmp."""
    res = client.get("/assets/icon/w0001.png")
    assert res.status_code == 200
    assert res.content[:4] == b"\x89PNG", "ожидается PNG"


def test_icon_route_rejects_traversal(client: TestClient) -> None:
    for bad in ("../secret", "..%2Fsecret", "a/b"):
        res = client.get(f"/assets/icon/{bad}")
        assert res.status_code in (400, 404), bad


def test_assets_dir_is_resolved_automatically() -> None:
    """Без переменных окружения каталог должен находиться сам,
    иначе все иконки отдают 404."""
    from app.routers.cards import assets_dir

    assert assets_dir().is_dir(), (
        "каталог ресурсов не найден: задайте ARBAH_ASSETS_DIR"
    )


def test_monster_card_links_to_creature(client: TestClient) -> None:
    """Карточка монстра из monsterlist должна находить существо по имени."""
    body = client.get("/api/catalog/monsters/2/full").json()
    assert body["name"] == "Sleepy Snail"
    assert body["creature_id"] is not None
    assert body["short_name"]