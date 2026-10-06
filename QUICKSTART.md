# Быстрый старт - Tales of Pirate на Linux

## TL;DR - Минимальные шаги для запуска

```bash
# 1. Клонировать репозиторий
cd TalesOfPirateDX9

# 2. Запустить через Docker Compose
docker-compose up -d

# 3. Проверить статус
docker-compose ps
docker-compose logs -f wine-server
```

## Что происходит:

1. **MSSQL контейнер** запускается и инициализирует базы данных
2. **Wine контейнер** запускает 4 серверных процесса:
   - AccountServer (порт 1978)
   - GroupServer (порт 1975)
   - GateServer (порт 1973)
   - GameServer (порт 1971)

## Подключение клиента:

### Вариант 1: Клиент на той же машине
```bash
cd Client/system
wine Game.exe pKcfT0PcaX
```

### Вариант 2: Клиент на другой машине
Отредактировать конфигурацию клиента (если есть), указав IP сервера.

## Проверка работы:

```bash
# Проверить логи серверов
docker-compose logs -f wine-server

# Проверить запущенные процессы
docker-compose exec wine-server ps aux | grep wine

# Проверить открытые порты
docker-compose exec wine-server netstat -tlnp
```

## Остановка:

```bash
docker-compose down
```

## Удаление данных:

```bash
docker-compose down -v  # Удалит volumes с БД
```

## Проблемы и решения:

### MSSQL не запускается
```bash
# Увеличить память Docker (минимум 2GB для MSSQL)
# Проверить логи
docker-compose logs mssql
```

### Wine серверы не стартуют
```bash
# Пересобрать образ
docker-compose build --no-cache wine-server

# Проверить инициализацию Wine
docker-compose exec wine-server bash
/opt/scripts/init-wine.sh
```

### Клиент не подключается
- Проверить firewall (порты 1973, 1975, 1978)
- Проверить что GateServer слушает правильный интерфейс
- Проверить логи GateServer: `docker-compose logs wine-server | grep GateServer`

## Структура файлов:

```
docker-compose.yml          # Оркестрация контейнеров
docker/
├── Dockerfile.wine         # Wine сервер
├── Dockerfile.mssql        # MSSQL сервер
├── Dockerfile.build        # Сборка из исходников
├── init-wine.sh           # Инициализация Wine
├── start-servers.sh       # Запуск серверов
├── setup-db.sh            # Настройка БД
└── build-project.sh       # Сборка проекта
```

## Дополнительные команды:

```bash
# Войти в контейнер
docker-compose exec wine-server bash

# Перезапустить серверы
docker-compose restart wine-server

# Посмотреть размер volumes
docker system df -v

# Очистить старые образы
docker image prune -f
```

## Конфигурация под свои нужды:

Отредактировать файлы в `server/*/`:
- `AccountServer.cfg` - настройки аутентификации
- `GateServer.cfg` - настройки шлюза
- `GroupServer.cfg` - настройки групп
- `GameServer00.cfg` - настройки игрового мира

После изменений:
```bash
docker-compose restart wine-server
```
