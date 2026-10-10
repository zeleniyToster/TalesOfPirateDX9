"""Калькулятор боевых характеристик.

Формулы перенесены из игровых скриптов дословно, коэффициенты берутся
из :mod:`app.etl.coefficients` (они вытаскиваются прямо из
``AttrCalculate.lua``, поэтому рассинхрон с игрой исключён).

Источник формул — ``AttrCalculate.lua``, функция ``ExAttrCheck``:

    MxHP   = floor(Con * 3 * Mxhp_con_rad1 + Mxhp_con_rad2 * Con^Mxhp_lv_rad)
    MxSP   = floor(Sta * 3 * Mxsp_sta_rad1 + Mxsp_sta_rad2 * Sta^Mxsp_lv_rad)
    mnatk  = floor(Str * Mnatk_str_rad1 + Dex * Mnatk_dex_rad1
                   + Agi * Mnatk_str_rad2 + Luk * Mnatk_dex_rad2)
    mxatk  = floor(Str * Mxatk_str_rad1 + Dex * Mxatk_dex_rad1
                   + Agi * Mxatk_str_rad2 + Luk * Mxatk_dex_rad2)
    def    = floor(Con * 5 * Def_con_rad1 + Def_con_rad2 * Con^2)
    hit    = floor(Dex * Hit_dex_rad1 + Hit_dex_rad2) + Lv * 2 + 5
    flee   = floor(Agi * Flee_agi_rad1 + Flee_agi_rad2) + Lv * 2 + 5
    mf     = 100 + floor(Luk * 3 * Mf_luk_rad)
    crt    = 11  + floor(Luk * 3 * Crt_luk_rad)
    hrec   = max(max(2 * MxHP * Hrec_bsmxhp_rad + Con * 3 * Hrec_con_rad, ...), Hrec_min)
    srec   = max((MxSP * Srec_bsmxsp_rad + Sta * 3 * Srec_sta_rad) / 2, 1)
    aspd   = floor(100000 / min(floor(65 + Agi * Aspd_agi_rad), 300))

Калькулятор считает «голого» персонажа: без экипировки и баффов.
Это ровно тот случай, когда результат можно сверить с сервером.
"""

from __future__ import annotations

import json
import math
from functools import lru_cache
from pathlib import Path

from ..config import PROJECT_DIR

COEFF_PATH = PROJECT_DIR / "backend" / "app" / "data" / "coefficients.json"

# Скорость передвижения и уклонение в игре не выводятся из шести статов,
# а приходят из состояний и экипировки. Здесь они отдаются как есть,
# чтобы калькулятор не подсовывал правдоподобные, но выдуманные числа.
DEFAULTS = {"move_speed": 0, "evasion": 0}


@lru_cache(maxsize=1)
def load_coefficients() -> dict:
    """Читает выгруженные коэффициенты (с диска, с кэшем на процесс)."""
    if not COEFF_PATH.is_file():
        raise FileNotFoundError(
            f"Файл {COEFF_PATH} не найден. Соберите его командой:\n"
            f"    python -m app.etl.coefficients"
        )
    return json.loads(COEFF_PATH.read_text(encoding="utf-8"))


# Русские названия профессий из JobType.lua. Базовые восемь совпадают
# с оригиналом, ветки второго порядка — из комментариев того же файла.
JOB_NAMES_RU = {
    0: "Новичок",
    1: "Мечник",
    2: "Охотник",
    3: "Моряк",
    4: "Искатель приключений",
    5: "Знахарь",
    6: "Ремесленник",
    7: "Торговец",
    8: "Чемпион",
    9: "Рыцарь",
    10: "Белый рыцарь",
    11: "Зверолов",
    12: "Стрелок",
    13: "Жрец",
    14: "Мастер печатей",
    15: "Капитан",
    16: "Мореход",
    17: "Выскочка",
    18: "Инженер",
}


def jobs() -> list[dict[str, object]]:
    """Список профессий: код, ключ и человекочитаемое название."""
    data = load_coefficients()
    out = []
    for code, key in sorted(data["jobs"].items(), key=lambda kv: int(kv[0])):
        numeric = int(code)
        out.append(
            {
                "code": numeric,
                "key": key,
                "name": JOB_NAMES_RU.get(
                    numeric, key.removeprefix("JOB_TYPE_").replace("_", " ").title()
                ),
            }
        )
    return out


class _Coeff:
    """Доступ к коэффициентам конкретной профессии."""

    def __init__(self, tables: dict, job: int) -> None:
        self._tables = tables
        self.job = str(job)

    def __call__(self, table: str, default: float = 0.0) -> float:
        return self._tables.get(table, {}).get(self.job, default)


def calculate(
    *,
    job: int,
    level: int,
    strength: int,
    dexterity: int,
    agility: int,
    constitution: int,
    stamina: int,
    luck: int,
    ship_level: int = 30,
    xp_rates: float = 1.0,
    drop_rates: float = 1.0,
) -> dict[str, int]:
    """Считает боевые характеристики «голого» персонажа."""
    c = _Coeff(load_coefficients()["tables"], job)

    # В Lua Con(Player) и подобные — целые (Lua положительные double
    # приводятся к int оператором +0), поэтому округляем вниз.
    con = int(constitution)
    sta = int(stamina)
    str_ = int(strength)
    dex = int(dexterity)
    agi = int(agility)
    luk = int(luck)

    # Lua math.floor(x * 3 / 20) — целочисленное деление.
    con_3_20 = con * 3 // 20
    sta_3_20 = sta * 3 // 20
    str_4_20 = str_ * 4 // 20
    dex_4_20 = dex * 4 // 20

    mxhp = math.floor(
        con * 3 * c("Mxhp_con_rad1")
        + c("Mxhp_con_rad2") * con_3_20**2
        + level * c("Mxhp_lv_rad")
        + 40
    )
    mxsp = math.floor(
        sta * 3 * c("Mxsp_sta_rad1")
        + c("Mxsp_sta_rad2") * sta_3_20**2
        + level * c("Mxsp_lv_rad")
        + 5
    )

    mnatk = math.floor(
        str_ * c("Mnatk_str_rad1")
        + dex * c("Mnatk_dex_rad1")
        + c("Mnatk_str_rad2") * str_4_20**2
        + c("Mnatk_dex_rad2") * dex_4_20**2
    )
    mxatk = math.floor(
        str_ * c("Mxatk_str_rad1")
        + dex * c("Mxatk_dex_rad1")
        + c("Mxatk_str_rad2") * str_4_20**2
        + c("Mxatk_dex_rad2") * dex_4_20**2
    )

    defence = math.floor(
        con * 5 * c("Def_con_rad1")
        + c("Def_con_rad2") * (con_3_20**2)
    )

    hit = math.floor(dex * c("Hit_dex_rad1")) + level * 2 + 5
    flee = math.floor(agi * c("Flee_agi_rad1")) + level * 2 + 5

    magic = 100 + math.floor(luk * 3 * c("Mf_luk_rad"))
    crit = 11 + math.floor(luk * 3 * c("Crt_luk_rad"))

    hp_recover = max(
        max(2 * mxhp * c("Hrec_bsmxhp_rad") + con * 3 * c("Hrec_con_rad"), 1),
        0,
    )
    sp_recover = max(
        (mxsp * c("Srec_bsmxsp_rad") + sta * 3 * c("Srec_sta_rad")) / 2,
        1,
    )

    aspd = math.floor(100000 / min(math.floor(65 + agi * c("Aspd_agi_rad")), 300))

    ship_defence = ship_level * 100
    ship_attack = ship_level * 10

    return {
        "job": job,
        "level": level,
        "max_hp": max(mxhp, 1),
        "max_sp": max(mxsp, 1),
        "min_attack": max(mnatk, 0),
        "max_attack": max(mxatk, 0),
        "defence": max(defence, 0),
        "resist": max(defence, 0),
        "hit": max(hit, 0),
        "flee": max(flee, 0),
        "magic": magic,
        "crit": crit,
        "hp_recover": int(hp_recover),
        "sp_recover": int(sp_recover),
        "attack_speed": aspd,
        "move_speed": DEFAULTS["move_speed"],
        "evasion": DEFAULTS["evasion"],
        "ship_defence": ship_defence,
        "ship_attack": ship_attack,
        "exp_current": 0,
        "exp_to_next": 0,
    }