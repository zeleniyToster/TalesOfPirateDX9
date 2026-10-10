FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV WINEARCH=win32
ENV WINEPREFIX=/wine
ENV WINEDEBUG=-all
ENV DISPLAY=:0
ENV WINEDLLOVERRIDES="mscoree=d;mshtml=d"

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
        wget \
        gnupg2 \
        software-properties-common \
        xvfb \
        winbind \
        cabextract \
        winetricks && \
    mkdir -pm755 /etc/apt/keyrings && \
    wget -O /etc/apt/keyrings/winehq-archive.key \
        https://dl.winehq.org/wine-builds/winehq.key && \
    wget -O /etc/apt/sources.list.d/winehq-noble.sources \
        https://dl.winehq.org/wine-builds/ubuntu/dists/noble/winehq-noble.sources && \
    apt-get update && \
    apt-get install -y --install-recommends winehq-stable && \
    rm -rf /var/lib/apt/lists/*

COPY msodbcsqlx86_17.10.1.msi /tmp/msodbcsqlx86_17.10.1.msi

RUN set -eux; \
    Xvfb :0 -screen 0 640x480x16 >/tmp/xvfb.log 2>&1 & \
    xvfb_pid=$!; \
    trap 'kill "$xvfb_pid" 2>/dev/null || true' EXIT; \
    sleep 2; \
    wineboot --init; \
    wineserver -w; \
    winetricks -q --force vcrun2022; \
    wineserver -w; \
    IACCEPTMSODBCSQLLICENSETERMS=YES \
        wine msiexec /i /tmp/msodbcsqlx86_17.10.1.msi /qn /norestart \
        IACCEPTMSODBCSQLLICENSETERMS=YES; \
    wineserver -w; \
    wine reg query "HKLM\\SOFTWARE\\ODBC\\ODBCINST.INI\\ODBC Drivers"; \
    wineserver -w; \
    rm -f /tmp/msodbcsqlx86_17.10.1.msi

WORKDIR /app

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
