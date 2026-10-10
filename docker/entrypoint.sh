#!/bin/sh
set -eu
rm -f /tmp/.X0-lock
Xvfb :0 -screen 0 640x480x16 >/tmp/xvfb.log 2>&1 &
sleep 1
export WINEDEBUG=-all,err+all
exec stdbuf -oL -eL wine "$@"