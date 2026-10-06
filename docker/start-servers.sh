#!/bin/bash

# Скрипт запуска серверов Tales of Pirate под Wine
set -e

export WINEPREFIX=/home/wineuser/.wine
export WINEARCH=win32

# Инициализация при первом запуске
/opt/scripts/init-wine.sh

echo "========================================="
echo " Tales of Pirate - Docker Server"
echo "========================================="

cd /opt/server

# Функция запуска сервера
start_server() {
    local name=$1
    local dir=$2
    local cfg=$3
    
    echo "Запуск $name..."
    cd "/opt/server/$dir"
    wine "${dir}.exe" "${cfg}" >> "/opt/logs/${name}.log" 2>&1 &
    echo "$name запущен (PID: $!)"
    cd /opt/server
}

# Ожидание доступности MSSQL
echo "Ожидание MSSQL..."
for i in $(seq 1 30); do
    if nc -z mssql 1433 2>/dev/null; then
        echo "MSSQL доступен!"
        break
    fi
    echo "Ожидание MSSQL... ($i/30)"
    sleep 2
done

if ! nc -z mssql 1433 2>/dev/null; then
    echo "ОШИБКА: MSSQL недоступен!"
    exit 1
fi

# Инициализация БД при первом запуске
if [ ! -f /opt/logs/.db_initialized ]; then
    echo "Инициализация базы данных..."
    /opt/scripts/setup-db.sh
    touch /opt/logs/.db_initialized
fi

# Запуск серверов в правильном порядке
echo ""
echo "=== Запуск серверов ==="

# 1. AccountServer - первый, обрабатывает логины
start_server "AccountServer" "AccountServer" "AccountServer.cfg"
sleep 3

# 2. GroupServer - координирует игровые серверы
start_server "GroupServer" "GroupServer" "GroupServer.cfg"
sleep 3

# 3. GateServer - шлюз для клиентов
start_server "GateServer" "GateServer" "GateServer.cfg"
sleep 3

# 4. GameServer - основной игровой сервер
start_server "GameServer" "GameServer" "GameServer00.cfg"
sleep 2

echo ""
echo "=== Все серверы запущены ==="
echo ""
echo "Порты:"
echo "  GateServer:    1973 (клиенты)"
echo "  GroupServer:   1975"
echo "  AccountServer: 1978"
echo ""
echo "Логи: /opt/logs/"
echo ""
echo "Для остановки: docker-compose down"
echo "Для просмотра логов: docker-compose logs -f wine-server"
echo ""

# Ожидаем все процессы
wait
