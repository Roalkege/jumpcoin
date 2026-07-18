# Jumpcoin Docker

Single-container deployment of the Jumpcoin Qt wallet, accessible from any
modern browser through **noVNC**. The container also runs `jumpcoind` so the
Qt wallet has a local node to talk to out of the box.

## Architecture

```
   Browser  ──HTTP/WS──▶  noVNC (:6080)
                              │
                              ▼
                         websockify
                              │
                              ▼
                         x0vncserver (:5900)  ◀── TigerVNC
                              │
                              ▼
   Xvfb (:99) ◀── openbox  ◀── jumpcoin-qt  ──RPC──▶  jumpcoind
                                                          │
                                                          ▼
                                                       P2P (:31242)
```

Everything is supervised by `supervisord` and runs as the unprivileged
`jumpcoin` user.

## Quick start

```bash
# 1. Build the image (compiles jumpcoind + jumpcoin-qt from this tree)
docker compose build

# 2. Start in the background
docker compose up -d

# 3. Open the wallet in your browser
xdg-open http://localhost:6080/vnc.html      # Linux
open      http://localhost:6080/vnc.html      # macOS
start     http://localhost:6080/vnc.html      # Windows
```

Click **Connect** on the noVNC page. The full Qt wallet UI is rendered inside
the page.

## Ports

| Port  | Purpose                          | Default binding |
| ----- | -------------------------------- | --------------- |
| 6080  | noVNC web UI                     | `0.0.0.0`       |
| 5900  | Raw VNC (e.g. TigerVNC, Remmina) | `0.0.0.0`       |
| 31240 | `jumpcoind` RPC                  | `127.0.0.1`     |
| 31242 | `jumpcoind` P2P                  | `127.0.0.1`     |

Adjust the bindings in `docker-compose.yml` to taste. To expose RPC/P2P on
your LAN, change `127.0.0.1:` to your host IP or remove the prefix.

## Persistence

Blockchain, wallet and config live in the named volume `jumpcoin-data` which
is mounted at `/home/jumpcoin/.jumpcoin` inside the container. To back up:

```bash
docker run --rm -v jumpcoin-data:/data -v "$PWD":/backup \
    ubuntu tar czf /backup/jumpcoin-data.tgz -C /data .
```

To start over from scratch: `docker volume rm jumpcoin-data`.

## Configuration

### Why is `disablewallet=1` set in `jumpcoin.conf`?

Both `jumpcoind` and `jumpcoin-qt` are running inside the container. If both
processes try to lock `wallet.dat`, one of them dies with
`Cannot obtain a lock on data directory`. The fix: the headless daemon runs
**without** the wallet (`disablewallet=1`), the Qt GUI runs **with** the
wallet. From the user's point of view nothing changes - the wallet is fully
functional through the Qt UI, the daemon just acts as a P2P/RPC node.

If you need CLI access to the wallet (e.g. `jumpcoin-cli sendtoaddress ...`),
set `WALLET_FROM_DAEMON=1` in `docker-compose.yml` and remove
`disablewallet=1` from the generated config. The Qt wallet will then connect
to the daemon's wallet via RPC instead of opening the file itself.

All knobs live as environment variables in `docker-compose.yml`:

| Variable           | Default       | Notes                                     |
| ------------------ | ------------- | ----------------------------------------- |
| `RPC_USER`         | `jumpcoin`    | RPC user                                  |
| `RPC_PASSWORD`     | `jumpcoin`    | **Change me** before going public         |
| `RPC_PORT`         | `31240`       |                                           |
| `P2P_PORT`         | `31242`       |                                           |
| `WALLET_RPCALLOWIP`| `127.0.0.1`   | CIDR list                                 |
| `VNC_PORT`         | `5900`        |                                           |
| `NOVNC_PORT`       | `6080`        |                                           |
| `DISPLAY_WIDTH`    | `1280`        |                                           |
| `DISPLAY_HEIGHT`   | `800`         |                                           |
| `DISPLAY_DEPTH`    | `24`          |                                           |
| `VNC_PASSWORD`     | _(unset)_     | Set to require VNC auth                   |
| `DAEMON_ARGS`      | _(unset)_     | Extra `jumpcoind` CLI args, e.g. pruning  |
| `TOR_BOOTSTRAP_ENABLED` | `true`   | Add bundled Tor v3 bootstrap peers        |
| `CLEARNET_BOOTSTRAP_ENABLED` | `true` | Add bundled clearnet bootstrap peers   |

The generated `jumpcoin.conf` is written once on first start. If you want
to start over, mount your own `jumpcoin.conf` into
`/home/jumpcoin/.jumpcoin/jumpcoin.conf` and the entrypoint will leave it
alone.

## CLI access to the running daemon

```bash
docker exec -it jumpcoin-qt \
    jumpcoin-cli -conf=/home/jumpcoin/.jumpcoin/jumpcoin.conf getinfo
```

## File layout

```
Dockerfile                - multi-stage build (compile + runtime)
docker-compose.yml        - service definition, volumes, port mappings
docker/entrypoint.sh      - generates jumpcoin.conf + VNC password, execs supervisord
docker/supervisord.conf   - process tree (xvfb, openbox, vnc, novnc, daemon, qt)
docker/menu.xml           - right-click menu for openbox
.dockerignore             - keeps the build context small
```

## Why build Berkeley DB 4.8 from source?

The wallet code targets BDB 4.8 specifically. Ubuntu 22.04 only ships BDB 5.3,
which would either fail to build or, with `--with-incompatible-bdb`, produce
`wallet.dat` files that aren't portable across BDB versions. The `contrib/install_db4.sh`
script (provided by the upstream project) downloads and builds BDB 4.8.30 cleanly,
so the build is reproducible and `wallet.dat` files are portable.


## Logs

`supervisord` writes per-process logs into `/var/log/supervisor/`:

```bash
docker exec jumpcoin-qt tail -f /var/log/supervisor/jumpcoin-qt.log
```

Or stream them all:

```bash
docker logs -f jumpcoin-qt
```

## Image size & build time

- Cold build: ~10-20 min on a 4-core machine (compiles Qt-wallet from source)
- Image size: ~1.3 GB (Ubuntu 22.04 + Qt5 + TigerVNC + noVNC)
- Re-builds are fast: only the `COPY . .` layer changes when you touch
  source files
