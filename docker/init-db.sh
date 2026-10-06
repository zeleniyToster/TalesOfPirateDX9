#!/bin/bash

# Скрипт инициализации MSSQL контейнера
# Запускает стандартный entrypoint MSSQL + выполняет инициализацию БД

# Запускаем инициализацию БД в фоне (после старта MSSQL)
(
    sleep 15  # Ждём пока MSSQL запустится
    /opt/mssql-scripts/init-db.sh
) &

# Запускаем стандартный MSSQL entrypoint
exec /opt/mssql/bin/sqlservr
