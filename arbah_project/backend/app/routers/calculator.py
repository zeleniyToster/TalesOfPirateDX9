"""Калькулятор боевых характеристик."""

from __future__ import annotations

from fastapi import APIRouter

from ..schemas import StatsInput, StatsOutput
from ..services.calculator import calculate, jobs

router = APIRouter(prefix="/calculator", tags=["calculator"])


@router.get("/jobs", summary="Список профессий для выпадающего списка")
def job_list() -> list[dict[str, object]]:
    return jobs()


@router.post("/stats", response_model=StatsOutput, summary="Боевые характеристики")
def stats(payload: StatsInput) -> StatsOutput:
    """Считает статы «голого» персонажа по формулам игры.

    Формулы перенесены из ``AttrCalculate.lua``, коэффициенты выгружаются
    из того же файла, поэтому совпадение с сервером ожидается точное
    (проверить стоит на реальном персонаже без экипировки).
    """
    result = calculate(
        job=payload.job,
        level=payload.level,
        strength=payload.strength,
        dexterity=payload.dexterity,
        agility=payload.agility,
        constitution=payload.constitution,
        stamina=payload.stamina,
        luck=payload.luck,
        ship_level=payload.ship_level,
        xp_rates=payload.xp_rates,
        drop_rates=payload.drop_rates,
    )
    return StatsOutput(**result)