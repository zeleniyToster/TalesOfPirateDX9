# Синтаксис образа: docker/dockerfile:1
# Зависимости ставятся отдельным слоем поверх requirements.txt,
# поэтому правка кода не пересобирает pip.

FROM python:3.13-slim AS base

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1 \
    PIP_NO_CACHE_DIR=1 \
    PIP_DISABLE_PIP_VERSION_CHECK=1

WORKDIR /app

# tini нужен, чтобы uvicorn корректно обрабатывал SIGTERM от docker stop:
# без него контейнер убивается по таймауту вместо штатного завершения.
RUN apt-get update \
 && apt-get install -y --no-install-recommends tini \
 && rm -rf /var/lib/apt/lists/*

COPY backend/requirements.txt /app/backend/requirements.txt
RUN pip install -r /app/backend/requirements.txt

COPY backend /app/backend
COPY frontend /app/frontend
COPY entrypoint.sh /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh

# Игровые таблицы в образ НЕ запекаются. Они приходят томом из
# server/GameServer/resource, поэтому правка предметов, дропа или
# расписания данжей не требует пересборки образа: достаточно
# перезапустить контейнер или вызвать POST /api/admin/refresh.
# Копия таблиц в каталоге data/ остаётся только для разработки
# на Windows, где контейнера нет.
ENV ARBAH_DATA_DIR=/srv/resource \
    ARBAH_DB_PATH=/var/lib/arbah/arbah.db \
    ARBAH_FRONTEND_DIR=/app/frontend \
    ARBAH_ASSETS_DIR=/srv/assets \
    PYTHONPATH=/app/backend

# Каталог базы создаётся до смены пользователя: Docker переносит
# владельца из образа в свежий именованный том, иначе arbah не смог бы
# создать файл базы. Ресурсы клиента (иконки, текстуры) не копируются:
# их десятки тысяч файлов, они приходят томом.
RUN mkdir -p /var/lib/arbah /srv/assets \
 && useradd --system --uid 10001 --no-create-home arbah \
 && chown -R arbah:arbah /app /var/lib/arbah
USER arbah

# Именованный том переживает пересоздание контейнера: база не
# пересобирается при каждом docker compose up.
VOLUME ["/var/lib/arbah"]

EXPOSE 8000

HEALTHCHECK --interval=30s --timeout=5s --start-period=30s --retries=3 \
  CMD python -c "import urllib.request,sys; sys.exit(0 if urllib.request.urlopen('http://127.0.0.1:8000/api/health', timeout=4).status==200 else 1)"

ENTRYPOINT ["/app/entrypoint.sh"]
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000", "--proxy-headers", "--forwarded-allow-ips", "*"]