# Запуск Tales of Pirate на Linux через Docker + Wine

## Структура проекта

### Компоненты сервера:
1. **AccountServer** - сервер аутентификации (порт 1978)
2. **GateServer** - шлюзовой сервер (порт 1973 для клиентов, 1971 для GameServer)
3. **GroupServer** - сервер групп/гильдий (порт 1975)
4. **GameServer** - игровой сервер (порт 1971)

### Клиент:
- **Game.exe** - клиент игры с DirectX 9

### База данных:
- Microsoft SQL Server (требуются базы: AccountServer, GameDtopnet)

## Процесс сборки (Windows)

Проект собирается через Visual Studio 2013/2015 с помощью bat-файлов:

```bash
# Сборка серверных компонентов
sources/Server/AccountServer/AccountServer.bat    # Сборка AccountServer.sln
sources/Server/GameServer/GameServer.bat          # Сборка GameServer.sln
sources/Server/GateServer/GateServer.bat          # Сборка GateServer.sln
sources/Server/GroupServer/GroupServer.bat        # Сборка GroupServer.sln

# Сборка клиента
sources/Client/proj/kop.sln                       # Через Visual Studio

# Сборка библиотек
sources/Engine/proj/MindPower3D.sln              # Движок
sources/Libraries/*/Proj/*.sln                    # Библиотеки
```

Все bat-файлы вызывают: `call devenv Proj/<Name>.sln /build Release`

## Запуск на Linux через Docker + Wine

### Предварительные требования:
- Docker и Docker Compose
- Минимум 4GB RAM
- 10GB свободного места

### Быстрый старт:

```bash
# 1. Клонировать репозиторий
git clone <repository-url>
cd TalesOfPirateDX9

# 2. Собрать Docker образ
docker-compose build

# 3. Запустить MSSQL и серверы
docker-compose up -d mssql
docker-compose up -d wine-server

# 4. Проверить логи
docker-compose logs -f wine-server
```

### Структура Docker-контейнеров:

1. **mssql** - Microsoft SQL Server 2017/2019 для Linux
2. **wine-server** - Wine окружение для запуска Windows executables

### Порты:
- 1973/TCP - GateServer (клиентские подключения)
- 1975/TCP - GroupServer
- 1978/TCP - AccountServer
- 1433/TCP - MSSQL

### Инициализация базы данных:

```bash
# Подключиться к MSSQL контейнеру
docker-compose exec mssql /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P 'YourStrong!Passw0rd'

# Выполнить SQL скрипты в порядке:
# 1. mssql/[0]AccountServer.sql
# 2. mssql/[1]GameDB.sql
# 3. mssql/[2]Guild.sql
# 4. mssql/[3]Create SQL Login.sql
# 5. mssql/[4]Create Account.sql
```

### Запуск серверов (внутри wine-server контейнера):

```bash
docker-compose exec wine-server bash

# Внутри контейнера:
cd /opt/server

# Запустить в правильном порядке:
wine AccountServer/AccountServer.exe AccountServer/AccountServer.cfg &
wine GroupServer/GroupServer.exe GroupServer/GroupServer.cfg &
wine GateServer/GateServer.exe GateServer/GateServer.cfg &
wine GameServer/GameServer.exe GameServer/GameServer00.cfg &
```

### Запуск клиента:

```bash
# На хост-машине (требует Wine)
cd Client/system
wine Game.exe pKcfT0PcaX
```

Или через Docker:
```bash
docker-compose exec wine-server wine /opt/Client/system/Game.exe pKcfT0PcaX
```

## Конфигурация

### server/AccountServer/AccountServer.cfg
```ini
[net]
listen_port = 1978
listen_ip = 0.0.0.0  # Изменить с 127.0.0.1 для внешних подключений

[db]
dbserver = mssql  # Адрес MSSQL контейнера
db = AccountServer
userid = mothannakh
passwd = Y87dc#$98
```

### server/GateServer/GateServer.cfg
```ini
[ToClient]
IP = 0.0.0.0  # Слушать все интерфейсы
Port = 1973
```

### server/GroupServer/GroupServer.cfg
```ini
[Main]
Listen_IP = 0.0.0.0  # Изменить с 127.0.0.1
```

### server/GameServer/GameServer00.cfg
```ini
[Database]
db_ip = mssql  # Адрес MSSQL контейнера
```

## Troubleshooting

### Проблемы с Wine:
```bash
# Установить дополнительные зависимости
docker-compose exec wine-server bash
winetricks vcrun2013 vcrun2015 dotnet48
```

### Проблемы с подключением к MSSQL:
```bash
# Проверить доступность MSSQL из wine-server
docker-compose exec wine-server telnet mssql 1433
```

### Проблемы с DirectX:
```bash
# Установить DirectX через winetricks
docker-compose exec wine-server winetricks directx9
```

## Альтернатива: Native Linux build

Для production использования рекомендуется портировать сервер на Linux:
- Заменить Windows-specific API (Winsock, threading) на POSIX
- Использовать Unix domain sockets для IPC
- Нативная поддержка ODBC для MSSQL

## Ресурсы

- Официальная документация Wine: https://www.winehq.org/docs/
- Docker MSSQL: https://hub.docker.com/_/microsoft-mssql-server
- Visual Studio решения: sources/talesofpirates.sln (главный файл)
