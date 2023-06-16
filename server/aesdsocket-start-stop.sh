#!/bin/sh

set -e

APP=aesdsocket
DAEMON_PATH=/usr/bin/aesdsocket
OPTS="-d"

case "$1" in
    start)
        echo "Starting"
        start-stop-daemon -S -n $APP -a $DAEMON_PATH -- $OPTS
        ;;
    stop)
        echo "Stopping"
        start-stop-daemon -K -n $APP
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac
exit 0
