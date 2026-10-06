#!/bin/bash

# Скрипт инициализации базы данных MSSQL
echo "=== Инициализация базы данных ==="

MSSQL_HOST="mssql"
MSSQL_USER="sa"
MSSQL_PASS="YourStrong!Passw0rd"
SQLCMD="/opt/mssql-tools18/bin/sqlcmd -S $MSSQL_HOST -U $MSSQL_USER -P $MSSQL_PASS -C"

# Ожидание готовности MSSQL
for i in $(seq 1 30); do
    if $SQLCMD -Q "SELECT 1" &>/dev/null; then
        echo "MSSQL готов!"
        break
    fi
    echo "Ожидание MSSQL... ($i/30)"
    sleep 2
done

# Создание логина для серверов
echo "Создание SQL логина..."
$SQLCMD -Q "
IF NOT EXISTS (SELECT * FROM sys.sql_logins WHERE name = 'mothannakh')
BEGIN
    CREATE LOGIN mothannakh WITH PASSWORD = 'Y87dc#\$98', DEFAULT_DATABASE = master;
    ALTER SERVER ROLE sysadmin ADD MEMBER mothannakh;
END
" 2>/dev/null || echo "Логин уже существует или ошибка создания"

# Выполнение SQL скриптов
echo "Выполнение SQL скриптов..."

if [ -f /opt/mssql-scripts/01-AccountServer.sql ]; then
    echo "  Создание базы AccountServer..."
    $SQLCMD -i /opt/mssql-scripts/01-AccountServer.sql 2>/dev/null || echo "  AccountServer DB уже существует"
fi

if [ -f /opt/mssql-scripts/02-GameDB.sql ]; then
    echo "  Создание базы GameDtopnet..."
    $SQLCMD -i /opt/mssql-scripts/02-GameDB.sql 2>/dev/null || echo "  GameDtopnet DB уже существует"
fi

if [ -f /opt/mssql-scripts/03-Guild.sql ]; then
    echo "  Создание таблиц гильдий..."
    $SQLCMD -i /opt/mssql-scripts/03-Guild.sql 2>/dev/null || echo "  Guild tables уже существуют"
fi

# Назначение владельца БД
echo "Назначение прав..."
$SQLCMD -Q "
USE AccountServer;
IF NOT EXISTS (SELECT * FROM sys.database_principals WHERE name = 'mothannakh')
    CREATE USER mothannakh FOR LOGIN mothannakh;
EXEC sp_addrolemember 'db_owner', 'mothannakh';

USE GameDtopnet;
IF NOT EXISTS (SELECT * FROM sys.database_principals WHERE name = 'mothannakh')
    CREATE USER mothannakh FOR LOGIN mothannakh;
EXEC sp_addrolemember 'db_owner', 'mothannakh';
" 2>/dev/null || echo "Права уже назначены"

echo "=== Инициализация БД завершена ==="
