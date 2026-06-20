#!/usr/bin/env bash
set -euo pipefail

auth_args=()
if [[ "${VNC_SECURITY_TYPES:-}" == "None" ]]; then
    auth_args=(-nopw)
else
    auth_args=(-rfbauth "${HOME}/.vnc/passwd")
fi

exec /usr/bin/x11vnc \
    -display "${DISPLAY:-:99}" \
    -rfbport "${VNC_PORT:-5900}" \
    -forever \
    -shared \
    -listen 0.0.0.0 \
    -noshm \
    "${auth_args[@]}"
