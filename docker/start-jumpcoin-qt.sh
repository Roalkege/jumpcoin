#!/usr/bin/env bash
# Wait for the bundled Tor control cookie before starting the wallet.  Without
# this guard supervisord can start jumpcoin-qt a few milliseconds before Tor
# creates the cookie, leaving the legacy Tor controller unable to authenticate.

set -euo pipefail

TOR_COOKIE="/var/lib/tor/control_auth_cookie"

if [[ -x /usr/bin/tor ]]; then
    for _attempt in $(seq 1 300); do
        if [[ -r "${TOR_COOKIE}" ]]; then
            break
        fi
        sleep 0.1
    done

    if [[ ! -r "${TOR_COOKIE}" ]]; then
        printf '[jumpcoin-qt] WARNING: Tor control cookie was not readable after 30 seconds\n' >&2
    fi
fi

exec /usr/local/bin/jumpcoin-qt \
    -conf="${HOME}/.jumpcoin/jumpcoin.conf" \
    -datadir="${HOME}/.jumpcoin" \
    -printtoconsole
