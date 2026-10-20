# -*- coding: utf-8 -*-
"""Разбор реального расписания данжей из SetMapEntryTime.

Файл: server/GameServer/resource/<карта>/ctrl.lua
Вызов (расшифрован по Expand.h:3452):

    SetMapEntryTime(map, "год/мес/день/час/мин",
                            "д/ч/м",     интервал
                            "д/ч/м",     сколько жив вход
                            "д/ч/м")     сколько живёт карта

Старый разбор смотрел на can_open_entry_<карта> и сравнение
``Hour == N``, но это legacy-проверка: реальное расписание задаёт
SetMapEntryTime, и после правки одного расписания другое продолжало
показывать старые часы.
"""
from __future__ import annotations

import re
from typing import Iterator
from pathlib import Path

SET_TIME = re.compile(
    r'SetMapEntryTime\s*\(\s*map\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"\s*,'
    r'\s*"([^"]*)"\s*,\s*"([^"]*)"\s*\)'
)


def _dhm(text: str) -> int:
    """'0/4/30' -> секунды."""
    parts = text.split('/')
    if len(parts) != 3:
        return 0
    try:
        d, h, m = (int(p) for p in parts)
    except ValueError:
        return 0
    return (d * 24 + h) * 3600 + m * 60


def _stamp(text: str) -> int:
    """'2005/8/30/2/0' -> секунды от полуночи, минус полночь этого дня.

    Год, месяц и день задают точку отсчёта, с которой считается
    первое открытие. Для расписания важна только минута суток.
    """
    parts = text.split('/')
    if len(parts) != 5:
        return 0
    try:
        return int(parts[3]) * 3600 + int(parts[4]) * 60
    except ValueError:
        return 0


def parse_dungeon_schedules(resource_dir: Path) -> Iterator[dict]:
    """Отдаёт по одной записи на каждый ctrl.lua с расписанием."""
    if not resource_dir.is_dir():
        return

    for ctrl in sorted(resource_dir.glob("*/ctrl.lua")):
        text = ctrl.read_bytes().decode("gbk", "replace")
        match = SET_TIME.search(text)
        if not match:
            continue

        first = _stamp(match.group(1))
        interval = _dhm(match.group(2))
        entry = _dhm(match.group(3))
        life = _dhm(match.group(4))
        if interval <= 0:
            continue

        yield {
            "map_file": ctrl.parent.name,
            "always_open": 0,
            "first_sec": first,
            "interval_sec": interval,
            "entry_sec": entry,
            "life_sec": life,
            "hours": _hours(first, interval, life),
        }


def _hours(first: int, interval: int, life: int, limit: int = 24) -> str:
    """Часы открытия в течение суток: '0,2,4,6' для шага в два часа."""
    step = interval // 3600
    if step < 1 or step > limit:
        return ""
    out = [str((first // 3600 + k * step) % 24) for k in range(limit // step + 1)]
    return ",".join(dict.fromkeys(out))
