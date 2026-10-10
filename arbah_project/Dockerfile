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

# Исходные игровые таблицы. По умолчанию копируются из репозитория,
# но на проде каталог переопределяется через ARBAH_DATA_DIR (том).
COPY data/ /app/data/

# Ресурсы клиента (иконки, текстуры) НЕ копируются в образ:
# их десятки тысяч файлов, и они лежат на сервере рядом с игрой.
# Каталог подключается томом, путь задаётся через ARBAH_ASSETS_DIR.

ENV ARBAH_DATA_DIR=/app/data \
    ARBAH_DB_PATH=/app/data/arbah.db \
    ARBAH_FRONTEND_DIR=/app/frontend \
    ARBAH_ASSETS_DIR=/app/assets \
    PYTHONPATH=/app/backend

# База собирается на этапе сборки образа: контейнер стартует
# уже с готовыми данными, ETL при старте не нужен.
RUN python -m app.etl.coefficients \
 && python -m app.etl.ingest \
 && chmod -R a+rX /app/data

RUN useradd --system --uid 10001 --no-create-home arbah \
 && mkdir -p /app/data /app/assets \
 && chown -R arbah:arbah /app
USER arbah

EXPOSE 8000

HEALTHCHECK --interval=30s --timeout=5s --start-period=10s --retries=3 \
  CMD python -c "import urllib.request,sys; sys.exit(0 if urllib.request.urlopen('http://127.0.0.1:8000/api/health', timeout=4).status==200 else 1)"

ENTRYPOINT ["/usr/bin/tini", "--"]
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000", "--proxy-headers", "--forwarded-allow-ips", "*"]