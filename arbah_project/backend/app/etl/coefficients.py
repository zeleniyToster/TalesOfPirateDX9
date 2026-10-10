"""Извлекает игровые коэффициенты из Lua в JSON.

Источник истины — ``server/GameServer/resource/script/calculate/AttrCalculate.lua``,
где коэффициенты заданы строками вида::

    Mxhp_con_rad1[JOB_TYPE_XIANSHU], Mxhp_con_rad2[JOB_TYPE_XIANSHU] = 3, 2

Скрипт разбирает эти присваивания и кладёт результат в
``backend/app/data/coefficients.json``. Файл используется калькулятором,
поэтому пересчитывать его нужно только при правке игровых таблиц.

Запуск::

    python -m app.etl.coefficients [путь_к_AttrCalculate.lua]
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

from ..config import PROJECT_DIR

OUTPUT = PROJECT_DIR / "backend" / "app" / "data" / "coefficients.json"

# JOB_TYPE_* -> числовой код
JOB_LINE = re.compile(r"JOB_TYPE_(\w+)\s*=\s*(\d+)")

# Присваивание в стиле  Name[JOB], Name[JOB], ... = 1.5, 0.4, 0, 0
ASSIGN = re.compile(
    r"^([A-Za-z_]\w*)\s*\[\s*(JOB_TYPE_\w+)\s*\]\s*=\s*(.+?)\s*$"
)

# Ведущие нули в китайских комментариях и названия JOB_TYPE_* мешают split
COMMENT = re.compile(r"--.*$", re.M)


def _split_values(raw: str) -> list[float]:
    """Делит ``'1.5, 0.4, 0'`` на числа; ``1/200`` тоже поддерживается."""
    out: list[float] = []
    for chunk in raw.split(","):
        chunk = chunk.strip()
        if not chunk:
            continue
        if "/" in chunk:
            num, _, den = chunk.partition("/")
            try:
                out.append(float(num) / float(den))
                continue
            except ValueError:
                pass
        try:
            out.append(float(chunk))
        except ValueError:
            raise ValueError(f"не удалось разобрать значение: {chunk!r}")
    return out


def extract(lua_path: Path, jobtype_path: Path | None = None) -> dict:
    text = COMMENT.sub("", lua_path.read_text(encoding="gbk", errors="replace"))

    if jobtype_path is None:
        jobtype_path = lua_path.with_name("JobType.lua")

    jobs: dict[str, int] = {}
    if jobtype_path.is_file():
        job_text = jobtype_path.read_text(encoding="gbk", errors="replace")
        # Группа захвата не включает префикс JOB_TYPE_, а в таблицах
        # коэффициентов имена используются целиком — добавляем обратно.
        for name, code in JOB_LINE.findall(job_text):
            jobs["JOB_TYPE_" + name] = int(code)
    # Запасной вариант: константы могли остаться и в самом AttrCalculate.lua
    for name, code in JOB_LINE.findall(text):
        jobs.setdefault("JOB_TYPE_" + name, int(code))

    tables: dict[str, dict[str, float]] = {}

    for line in text.splitlines():
        line = line.strip()
        if "[" not in line or "=" not in line:
            continue

        left, _, right = line.partition("=")
        targets = re.findall(
            r"([A-Za-z_]\w*)\s*\[\s*(JOB_TYPE_\w+)\s*\]", left
        )
        if not targets:
            continue
        try:
            values = _split_values(right)
        except ValueError:
            continue
        if len(values) != len(targets):
            # Многострочные или вложенные выражения — пропускаем,
            # лучше не записать коэффициент, чем записать неверно.
            continue

        for (table_name, job_name), value in zip(targets, values):
            job_code = jobs.get(job_name)
            if job_code is None:
                continue
            tables.setdefault(table_name, {})[str(job_code)] = value

    return {"jobs": {str(v): k for k, v in jobs.items()}, "tables": tables}


def main(argv: list[str] | None = None) -> int:
    argv = sys.argv[1:] if argv is None else argv

    if argv:
        lua_path = Path(argv[0])
    else:
        # В контейнере исходники лежат рядом с таблицами в ARBAH_DATA_DIR,
        # при разработке — в репозитории с игровыми скриптами.
        from ..etl.ingest import resolve_data_dir

        lua_path = resolve_data_dir() / "AttrCalculate.lua"

    if not lua_path.is_file():
        print(f"Не найден {lua_path}", file=sys.stderr)
        return 1

    data = extract(lua_path)
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(
        json.dumps(data, ensure_ascii=False, indent=1, sort_keys=True),
        encoding="utf-8",
    )

    print(f"Источник: {lua_path}")
    print(f"Результат: {OUTPUT}")
    print(f"Профессий: {len(data['jobs'])}")
    print(f"Таблиц коэффициентов: {len(data['tables'])}")
    for name in sorted(data["tables"]):
        print(f"  {name:<22} {len(data['tables'][name])} значений")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())