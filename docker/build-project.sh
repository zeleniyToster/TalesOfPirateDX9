#!/bin/bash

# Скрипт сборки проекта Tales of Pirate под Wine
# Использует Visual Studio через Wine/Winetricks

set -e

export WINEPREFIX=/home/wineuser/.wine
export WINEARCH=win32

echo "========================================="
echo " Tales of Pirate - Сборка под Wine"
echo "========================================="
echo ""
echo "ВНИМАНИЕ: Сборка через Wine - экспериментальная функция."
echo "Рекомендуется использовать pre-built binaries из репозитория."
echo ""

# Проверка наличия Visual Studio
if [ ! -f "$WINEPREFIX/drive_c/Program Files/Microsoft Visual Studio 12.0/Common7/IDE/devenv.exe" ]; then
    echo "Visual Studio не найден в Wine prefix."
    echo ""
    echo "Для сборки необходимо:"
    echo "1. Установить Visual Studio 2013 в Wine prefix"
    echo "2. Или использовать cross-compile подход"
    echo ""
    echo "Альтернативы:"
    echo "  - Использовать MinGW cross-compiler (требует портирования)"
    echo "  - Собирать на Windows и копировать binaries"
    echo "  - Использовать Visual Studio в Windows VM"
    echo ""
    exit 1
fi

cd /opt/sources

echo "Сборка библиотек..."

# Сборка библиотек (порядок важен)
build_solution() {
    local sln=$1
    local name=$2
    echo "  Сборка $name..."
    cd "$(dirname $sln)"
    wine devenv "$(basename $sln)" /build Release 2>&1 | tee /opt/logs/build-$name.log
    cd /opt/sources
}

# Библиотеки
build_solution "Libraries/CaLua/CaLua.sln" "CaLua"
build_solution "Libraries/common/Proj/Common.sln" "Common"
build_solution "Libraries/Util/proj/util.sln" "Util"
build_solution "Libraries/EncLib/proj/EncLib.sln" "EncLib"

# Движок
build_solution "Engine/proj/MindPower3D.sln" "MindPower3D"

# Серверы
echo ""
echo "Сборка серверов..."
build_solution "Server/AccountServer/Proj/AccountServer.sln" "AccountServer"
build_solution "Server/GroupServer/Proj/GroupServer.sln" "GroupServer"
build_solution "Server/GateServer/Proj/GateServer.sln" "GateServer"
build_solution "Server/GameServer/Proj/GameServer.sln" "GameServer"

# Клиент
echo ""
echo "Сборка клиента..."
build_solution "Client/proj/kop.sln" "Client"

echo ""
echo "========================================="
echo " Сборка завершена!"
echo "========================================="
echo ""
echo "Логи сборки: /opt/logs/build-*.log"
echo ""
