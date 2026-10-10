"""Pydantic-схемы ответов API.

Вынесены отдельно, чтобы контракт фронтенда был описан в одном месте
и его можно было переиспользовать в тестах.
"""

from __future__ import annotations

from pydantic import BaseModel, Field


class Page(BaseModel):
    total: int = Field(description="Всего записей по фильтру")
    page: int
    page_size: int
    pages: int


class Item(BaseModel):
    item_id: int
    name: str
    icon: str | None = None
    type_id: int | None = None
    type_name: str | None = None
    forge_level: int | None = None
    price: int | None = None
    pile_max: int | None = None
    jobs: list[int] = Field(default_factory=list)
    required_level: int | None = None


class ItemList(BaseModel):
    page: Page
    items: list[Item]


class Monster(BaseModel):
    monster_id: int
    name: str
    level: int | None = None
    x: int | None = None
    y: int | None = None
    region: str | None = None


class MonsterList(BaseModel):
    page: Page
    monsters: list[Monster]


class Npc(BaseModel):
    npc_id: int
    name: str
    location: str | None = None
    x: int | None = None
    y: int | None = None
    region: str | None = None


class NpcList(BaseModel):
    page: Page
    npcs: list[Npc]


class Area(BaseModel):
    area_id: int
    name: str
    r: int | None = None
    g: int | None = None
    b: int | None = None
    is_city: bool = False


class MapOut(BaseModel):
    map_id: int
    file: str | None = None
    name: str
    x: int | None = None
    y: int | None = None


class LevelCost(BaseModel):
    level: int
    exp_total: int
    exp_cost: int


class StatsInput(BaseModel):
    """Вход калькулятора боевых характеристик.

    Принимаются итоговые базовые статы персонажа (то, что в игре
    показывает окно характеристик), а не распределение очков:
    производные статы в игре считаются именно от них, поэтому
    результат совпадает с серверным без калибровки.
    """

    job: int = Field(ge=0, le=18, description="Код профессии, см. /api/jobs")
    level: int = Field(ge=1, le=150, description="Уровень персонажа")

    strength: int = Field(ge=0, default=0, description="Сила (STR)")
    dexterity: int = Field(ge=0, default=0, description="Ловкость (DEX)")
    agility: int = Field(ge=0, default=0, description="Проворство (AGI)")
    constitution: int = Field(ge=0, default=0, description="Телосложение (CON)")
    stamina: int = Field(ge=0, default=0, description="Выносливость (STA)")
    luck: int = Field(ge=0, default=0, description="Удача (LUK)")

    ship_level: int = Field(ge=0, default=30, description="Уровень корабля")
    xp_rates: float = Field(gt=0, default=1.0, description="Множитель опыта")
    drop_rates: float = Field(gt=0, default=1.0, description="Множитель дропа")


class StatsOutput(BaseModel):
    job: int
    level: int

    max_hp: int
    max_sp: int
    min_attack: int
    max_attack: int
    defence: int
    resist: int
    hit: int
    flee: int
    magic: int
    crit: int
    hp_recover: int
    sp_recover: int
    attack_speed: int
    move_speed: int
    evasion: int

    exp_to_next: int
    exp_current: int
    ship_defence: int
    ship_attack: int