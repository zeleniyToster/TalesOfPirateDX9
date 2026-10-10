"""Общие мелочи роутеров: постраничная выдача и разбор параметров."""

from __future__ import annotations

from dataclasses import dataclass

from fastapi import HTTPException, Query

from ..config import settings
from ..schemas import Page


@dataclass(frozen=True)
class Paging:
    page: int
    page_size: int

    @property
    def offset(self) -> int:
        return (self.page - 1) * self.page_size

    def build(self, total: int) -> Page:
        pages = (total + self.page_size - 1) // self.page_size if total else 0
        return Page(
            total=total, page=self.page, page_size=self.page_size, pages=pages
        )


def paging(
    page: int = Query(1, ge=1, description="Номер страницы, с единицы"),
    page_size: int = Query(
        settings.page_size_default, ge=1, le=settings.page_size_max
    ),
) -> Paging:
    return Paging(page=page, page_size=page_size)


def order_by(sort: str, allowed: dict[str, str], default: str) -> str:
    """Проверяет поле сортировки по белому списку.

    Строка подставляется в SQL, поэтому всё, чего нет в ``allowed``,
    отбрасывается — инъекция через параметр сортировки не проходит.
    """
    column = allowed.get(sort)
    if column is None:
        if sort != default:
            raise HTTPException(
                status_code=400,
                detail=(
                    f"Сортировка по полю {sort!r} не поддерживается. "
                    f"Доступно: {', '.join(sorted(allowed))}."
                ),
            )
        column = allowed[default]
    return column