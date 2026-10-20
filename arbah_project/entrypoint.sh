#!/bin/sh
# Старт контейнера arbah.
#
# База собирается на лету из примонтированной папки сервера, а не
# запекается в образ. Благодаря этому правка игровых таблиц не требует
# пересборки образа: достаточно перезапустить контейнер или вызвать
# POST /api/admin/refresh.
set -e

: "${ARBAH_DATA_DIR:=/srv/arbah}"
: "${ARBAH_MAP_DIR:=/srv/resource}"
: "${ARBAH_DB_PATH:=/var/lib/arbah/arbah.db}"

if [ ! -d "$ARBAH_DATA_DIR" ]; then
    echo "[arbah] ОШИБКА: нет каталога с игровыми таблицами: $ARBAH_DATA_DIR" >&2
    echo "[arbah] проверьте, что в docker-compose указан том server/GameServer/resource" >&2
    exit 1
fi

# Каталог базы должен существовать и принадлежать пользователю arbah:
# Docker переносит владельца каталога из образа в свежий именованный том.
mkdir -p "$(dirname "$ARBAH_DB_PATH")"

LUA="$ARBAH_DATA_DIR/script/calculate/AttrCalculate.lua"
if [ -f "$LUA" ]; then
    echo "[arbah] разбираю коэффициенты $LUA"
    python -m app.etl.coefficients "$LUA" || echo "[arbah] коэффициенты не обновлены"
else
    echo "[arbah] AttrCalculate.lua не найден — беру коэффициенты из образа"
fi

echo "[arbah] собираю базу из $ARBAH_DATA_DIR"
ARBAH_MAP_DIR="$ARBAH_MAP_DIR" python -m app.etl.ingest "$ARBAH_DATA_DIR"

echo "[arbah] база готова: $ARBAH_DB_PATH"
echo "[arbah] запускаю веб-сервер"

# tini остаётся PID 1 и пере reap-ит зомби, сам веб-сервер запускается
# как дочерний процесс. Без него uvicorn был бы PID 1 и не убирал бы
# осиротевшие процессы.
exec /usr/bin/tini -- "$@"