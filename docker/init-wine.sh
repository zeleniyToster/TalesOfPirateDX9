#!/bin/bash

# Инициализация Wine prefix
echo "=== Инициализация Wine prefix ==="

export WINEPREFIX=/home/wineuser/.wine
export WINEARCH=win32

# Создаём prefix если его нет
if [ ! -d "$WINEPREFIX" ]; then
    echo "Создание нового Wine prefix..."
    wineboot --init
    sleep 5
fi

# Установка необходимых компонентов
echo "Установка Visual C++ Runtime..."
winetricks -q vcrun2013 2>/dev/null || echo "winetricks vcrun2013 пропущен (может быть уже установлен)"
winetricks -q vcrun2015 2>/dev/null || echo "winetricks vcrun2015 пропущен"

echo "=== Wine prefix инициализирован ==="

# Обновляем конфигурационные файлы для работы с MSSQL в Docker
echo "Обновление конфигурации для Docker..."
cd /opt

# Обновляем MSSQL адрес во всех конфигах
find /opt/server -name "*.cfg" -exec sed -i 's/127\.0\.0\.1/mssql/g' {} \;
# Для AccountServer - dbserver
sed -i 's/127\.0\.0\.1/mssql/g' /opt/server/AccountServer/AccountServer.cfg

# Создаём директории для логов
mkdir -p /opt/logs
mkdir -p /opt/server/GameServer/LOG

echo "Конфигурация обновлена."
echo "=== Инициализация завершена ==="
