# Запланированные изменения — отдельный рейт опыта для квестов

Статус: **отложено по решению владельца проекта.** Документ описывает идею и всё, что к ней
относится, чтобы вернуться к вопросу позже без повторного разбора кода.

Связанные документы: [`BUILD_NOTES.md`](BUILD_NOTES.md) — особенности сборки,
`../CHANGES.md` — хронология выполненных изменений.

---

## Идея

Сейчас квестовый опыт и обычный kill-опыт идут **разными путями**, и рейты на них
распрострастраняются по-разному. Хочется, чтобы квесты имели собственный множитель,
независимый от `Server.Rates.Global.EXP`.

По аналогии с уже существующим ключом это выглядело бы так:

```lua
Server.Rates.Global.MissionEXP = 1    -- Rate at which missions grant experience.
```

## Почему сейчас нельзя просто покрутить

Разберём, что происходит с опытом от убийства монстра:

```
CEventHandler::Event_ChaDie            EventCalculate.cpp:14 / :128
  → GetExp_New                          exp_and_level.lua:72
    → ShareTeamExp                      exp_and_level.lua:166
      → GetPlyExpRate                   exp_and_level.lua:247   ← глобальный рейт
        → ExpSystem                      functions.lua:6628
            → Server.Rates.Map[...]      functions.lua:6635-6636
            → Server.Rates.Day[...]      functions.lua:6638-6639
            → кап по Server.Level.Limit  functions.lua:6641-6642
```

Квестовый опыт идёт мимо всего этого:

```
ActionsProc                            MisSdk/MissionSdk.lua:3473-3479
    p1 = p1 * GetExpState(character) / 100    ← это НЕ рейт
  → AddExp                              CharScript.cpp:726-776
    → CFightAble::AddExpAndNotic        FightAble.cpp:2014
      → CFightAble::AddExp              FightAble.cpp:1979-2012
        → setAttr(ATTR_CEXP, …)
```

То есть квестовый опыт умножается **только** на `GetExpState` — это
`CCharacter::m_ExpScale` (`Character.h:713-714`), инициализируется 100 и меняется через
`CMD_PM_EXPSCALE` как штраф за лаг (`GameAppNet.cpp:753-848`). Живых ветвей там только
`100`: значения `50` и `0` закомментированы, так что множитель сейчас всегда равен 1.

Дополнительно: в квестовом пути **не проверяется потолок уровня**, поэтому большой квест
может «перепрыгнуть» уровень.

## Где именно вставлять

Одна точка — `MisSdk/MissionSdk.lua:3477`, ветка `AddExp` внутри `ActionsProc`:

```lua
actions[i].p1 = actions[i].p1 * GetExpState(character) / 100
actions[i].p2 = actions[i].p2 * GetExpState(character) / 100
```

Предлагаемая правка по смыслу:

```lua
actions[i].p1 = actions[i].p1 * GetExpState(character) / 100 * MISSION_EXP_RAID
actions[i].p2 = actions[i].p2 * GetExpState(character) / 100 * MISSION_EXP_RAID
```

с объявлением рядом с остальными рейтами в `variable.lua:30-36` и алиасом в том же стиле,
что уже есть на строках 532-539:

```lua
-- Server Rates
EXP_RAID = Server.Rates.Global.EXP
MF_RAID  = Server.Rates.Global.DROP
SetGlobalRates(MF_RAID,EXP_RAID)
TEAM_EXP_RAID = Server.Rates.Global.TeamEXP
ELF_EXP_GET_RAID = Server.Rates.Global.FairyEXP
SHIP_EXP_RAID = Server.Rates.Global.ShipEXP
Resource_RAID_ADJUST = Server.Rates.Global.Resource
```

## Что обязательно учесть при реализации

Ветку `AddExp` в `ActionsProc` покрывает **не все** способы выдачи квестового опыта.
Те же мимо-рейты идут:

| Функция | Файл:строка |
|---|---|
| `AddExpAndType` | `MissionSdk.lua:3533` → `CharScript.cpp:778` |
| `AddExpPer` | `functions.lua:5822` |
| `AddExpAll` | `functions.lua:5831` |
| `AddExp_1..6` | `functions.lua:6937-7077` |
| `QianAddExp` | `functions.lua:3585` |

Все они пишут `ATTR_CEXP` напрямую. Чтобы рейт работал везде, множитель надо провести
через каждую, а не только через `ActionsProc`. Обратная сторона: `ExpSystem` вызывается
**только** из `ShareTeamExp`, то есть только из kill-опыта, — поэтому «исправить» всё
через `ExpSystem` нельзя.

## Похожий случай, который уже есть

`Server.Rates.Global.MissionGold` объявлен в `variable.lua:35` с комментарием
«The rate that the player receives gold from missions» — но **нигде не читается**. Это
заготовка под ту же идею, сделанная в прошлом и не доведённая до конца. При реализации
стоит либо задействовать, либо удалить, чтобы не путать.

## Квестовые предметы — отдельная история

`GiveItem` (`MissionSdk.lua:3642` → `CharScript.cpp:1178`) кладёт предмет прямо в
инвентарь, без шансов и без рейтов. Если нужен и отдельный рейт для квестовых предметов,
точка та же — ветка `GiveItem` в `ActionsProc`.

При этом дроп предметов **для** квеста идёт через рейт: вторая ветка `Check_Baoliao`
в `FightAble.cpp:2226` использует `GetPlyDropRate`.

## Оценка сложности

Правка на одну функцию `ActionsProc` — около часа работы. Полное покрытие всех шести
способов выдачи — ближе к двум-трём часам, плюс тестирование на сервере. Конфиг при этом
остаётся в `variable.lua`, пересборка не нужна.
