#!/usr/bin/env bash
# launch-jumpcoin-qt.sh
#
# Helper to start the Jumpcoin-Qt wallet with the same flags supervisord
# uses.  Used by the desktop shortcut and the openbox menu so that the
# user can re-launch the wallet if it ever crashes.
#
# Delegates to supervisord via the Unix socket rather than spawning an
# untracked nohup child outside the supervision tree.

set -euo pipefail

SUPERVISORCTL="supervisorctl -s unix:///home/jumpcoin/.supervisor/supervisor.sock"

# Already running?  Nothing to do.
if pgrep -f jumpcoin-qt >/dev/null; then
    if command -v notify-send >/dev/null 2>&1; then
        notify-send "Jumpcoin" "Wallet is already running" 2>/dev/null || true
    fi
    exit 0
fi

# Ask supervisord to bring the process up.  'start' covers STOPPED/FATAL;
# if it fails (e.g. already STARTING), fall back to 'restart'.
$SUPERVISORCTL start jumpcoin-qt 2>/dev/null \
    || $SUPERVISORCTL restart jumpcoin-qt 2>/dev/null \
    || true
