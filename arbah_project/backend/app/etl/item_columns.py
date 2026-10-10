"""Карта колонок ``ItemInfo.txt``.

Файл читается движком ``CRawDataSet``: первый столбец — ID,
второй — имя, остальные попадают в ``ParamList`` начиная с третьего.
Дальше ``CItemRecordSet::_ReadRawDataInfo``
(``sources/Libraries/common/src/ItemRecord.cpp``) разбирает их
строго по порядку.

Заголовок самого ``.txt`` описывает только первые 15 столбцов и
частично врёт, поэтому здесь используется карта, восстановленная
из кода парсера. Эталон — ``defITEM_MODULE_NUM = 5``
(``ItemRecord.h:20``): именно пять колонок моделей, из-за чего
всё, что идёт дальше, сдвинуто на единицу относительно
«очевидного» чтения.

Ничего не угадывается: каждая позиция ниже соответствует
конкретной строке ``ParamList[m++]`` в исходнике.
"""

from __future__ import annotations

# Пять колонок моделей: Lance / Carsise / Phyllis / Ami + основная.
MODEL_COUNT = 5

C_ID = 0
C_NAME = 1
C_ICON = 2
C_MODEL = 3                      # 3, 4, 5, 6, 7
C_SHIP_FLAG = 8
C_HULL_MODEL = 9
C_TYPE = 10
C_PREFIX_CHANCE = 11
C_SET_ID = 12
C_FORGE_LEVEL = 13
C_FORGE_STEADY = 14
C_EXCLUSIVE_ID = 15
C_IS_TRADE = 16                  # можно торговать
C_IS_PICKUP = 17                 # можно подобрать
C_IS_THROW = 18                  # можно выбросить
C_IS_DELETE = 19                 # можно уничтожить
C_PILE_MAX = 20                  # размер стака
C_INSTANTIATED = 21
C_PRICE = 22
C_BODY_SIZE = 23
C_NEED_LEVEL = 24
C_JOB = 25
C_TITLE = 26
C_REPUTATION = 27
C_ABLE_LINK = 28                 # слоты, куда можно надеть
C_NEED_LINK = 29                 # слоты, которые нужно надеть
C_PICK_TO = 30                   # 0 — в инвентарь, 1 — в трюм

# Двадцать одноцифровых коэффициента идёт подряд: 31..51.
# Второй блок такого же размера — постоянные бонусы «мин,макс»: 52..72.
COEF_START = 31
VALU_START = COEF_START + 21     # 52

# Человекочитаемые названия бонусов — порядок совпадает с порядком
# колонок, задаётся первым же аргументом в коде парсера.
BONUS_FIELDS = [
    "STR", "AGI", "DEX", "CON", "STA", "LUK", "ASPD", "ADIS",
    "MNATK", "MXATK", "DEF", "MXHP", "MXSP", "FLEE", "HIT",
    "CRT", "MF", "HREC", "SREC", "MSPD", "COL",
]

BONUS_RU = {
    "STR": "Сила", "AGI": "Проворство", "DEX": "Ловкость",
    "CON": "Телосложение", "STA": "Выносливость", "LUK": "Удача",
    "ASPD": "Скорость атаки", "ADIS": "Дальность атаки",
    "MNATK": "Мин. атака", "MXATK": "Макс. атака",
    "DEF": "Защита", "MXHP": "Макс. HP", "MXSP": "Макс. SP",
    "FLEE": "Уклонение", "HIT": "Точность", "CRT": "Крит",
    "MF": "Найд", "HREC": "Реген. HP", "SREC": "Реген. SP",
    "MSPD": "Скорость бега", "COL": "Скорость сбора",
}

# Хвостовые поля.
C_PHYS_RESIST = VALU_START + len(BONUS_FIELDS)   # 73
C_LEFT_HAND = 74
C_ENERGY = 75
C_ENDURANCE = 76                                # прочность
C_HOLES = 77
C_SHIP_DUR_RECOVER = 78
C_SHIP_CANNONS = 79
C_SHIP_CREW = 80
C_SHIP_STD_CREW = 81
C_SHIP_CARGO = 82
C_SHIP_SUPPLY = 83
C_SHIP_SHELL_SPEED = 84
C_SHIP_MOVE_SPEED = 85
C_USE_EFFECT = 86                               # имя эффекта при использовании
C_DRAP = 87
C_ATTACH_EFFECT = 88
C_ATTACH_DUMMY = 89
C_ITEM_EFFECT = 90
C_AREA_EFFECT = 91
C_USE_ITEM_EFFECT = 92
C_DESCRIPTOR = 93                               # текст описания
C_COOLDOWN = 94

TOTAL_COLUMNS = 95

# Слоты экипировки в порядке enumEQUIP_* (Character.h).
# Важно: индексы не на единицу меньше привычного — LHAND это 6, а не 5,
# из-за чего «5» у ожерелья, а «9» у правой руки.
EQUIP_SLOTS = [
    "Головной убор",
    "Лицо",
    "Нагрудник",
    "Перчатки",
    "Обувь",
    "Ожерелье",
    "Левая рука",
    "Рука 1",
    "Рука 2",
    "Правая рука",
    "Аксессуар 1",
    "Аксессуар 2",
    "Аксессуар 3",
]

# Профессии по кодам из JobType.lua.
JOB_RU = {
    1: "Мечник", 2: "Охотник", 3: "Моряк", 4: "Искатель приключений",
    5: "Знахарь", 6: "Ремесленник", 7: "Торговец", 8: "Чемпион",
    9: "Рыцарь", 10: "Белый рыцарь", 11: "Зверолов", 12: "Стрелок",
    13: "Жрец", 14: "Мастер печатей", 15: "Капитан", 16: "Мореход",
    17: "Выскочка", 18: "Инженер",
}