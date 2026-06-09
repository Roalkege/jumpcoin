#!/usr/bin/env bash
# launch-jumpcoin-qt.sh
#
# Helper to start the Jumpcoin-Qt wallet with the same flags supervisord
# uses.  Used by the desktop shortcut and the openbox menu so that the
# user can re-launch the wallet if it ever crashes.
#
# If a jumpcoin-qt is already running this script is a no-op (the wallet
# will come back automatically via supervisord anyway).

set -u

CONF=/home/jumpcoin/.jumpcoin/jumpcoin.conf
DATADIR=/home/jumpcoin/.jumpcoin

# Already running?  Nothing to do.
if pgrep -f jumpcoin-qt >/dev/null; then
    if command -v notify-send >/dev/null 2>&1; then
        notify-send "Jumpcoin" "Wallet is already running" 2>/dev/null || true
    fi
    exit 0
fi

# Wait briefly for supervisord to bring it back on its own (autorestart).
for _ in 1 2 3 4 5; do
    if pgrep -f jumpcoin-qt >/dev/null; then exit 0; fi
    sleep 1
done

# Still not back?  Launch it ourselves with the same flags supervisord
# uses.  The process is reaped by supervisord (autorestart=true) on exit.
nohup /usr/local/bin/jumpcoin-qt \
    -conf="${CONF}" \
    -datadir="${DATADIR}" \
    -printtoconsole \
    >/home/jumpcoin/.supervisor/jumpcoin-qt.log 2>&1 &
disown || true
