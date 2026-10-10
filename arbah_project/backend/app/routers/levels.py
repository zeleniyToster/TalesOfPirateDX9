"""Таблица опыта и справочник профессий."""

from __future__ import annotations

from typing import Any

from fastapi import APIRouter, HTTPException, Query

from .. import db
from ..schemas import LevelCost
from ..services.calculator import calculate, jobs

router = APIRouter(prefix="/levels", tags=["levels"])


@router.get("/costs", response_model=list[LevelCost], summary="Стоимость уровней")
def level_costs(
    min_level: int | None = Query(None, ge=1),
    max_level: int | None = Query(None, ge=1),
) -> list[LevelCost]:
    where: list[str] = []
    args: list[Any] = []
    if min_level is not None:
        where.append("level >= ?")
        args.append(min_level)
    if max_level is not None:
        where.append("level <= ?")
        args.append(max_level)

    clause = (" WHERE " + " AND ".join(where)) if where else ""
    rows = db.query(
        f"SELECT level, exp_total, exp_cost FROM level_costs{clause} ORDER BY level",
        tuple(args),
    )
    return [
        LevelCost(level=r["level"], exp_total=r["exp_total"], exp_cost=r["exp_cost"])
        for r in rows
    ]


@router.get("/jobs", summary="Справочник профессий")
def job_list() -> list[dict[str, object]]:
    return jobs()


@router.get("/exp-to-level", summary="Сколько опыта до уровня")
def exp_to_level(
    current_level: int = Query(..., ge=1, le=150),
    current_exp: int = Query(0, ge=0, description="Опыт, уже накопленный на уровне"),
    rates: float = Query(1.0, gt=0, description="Множитель опыта сервера"),
) -> dict[str, int | float]:
    """Отвечает, сколько убийств/опыта нужно до следующего уровня.

    Учитывает, что уровень стоит разницу двух накопленных значений.
    """
    row = db.query_one(
        "SELECT exp_total FROM level_costs WHERE level = ?", (current_level + 1,)
    )
    if row is None:
        raise HTTPException(
            status_code=404,
            detail=f"Уровень {current_level + 1} выше максимального",
        )

    start = db.query_one(
        "SELECT exp_total FROM level_costs WHERE level = ?", (current_level,)
    )
    needed = row["exp_total"] - (start["exp_total"] if start else 0)
    left = max(0, needed - current_exp)
    return {
        "current_level": current_level,
        "next_level": current_level + 1,
        "needed": needed,
        "left": left,
        "left_with_rates": int(left / rates),
    }