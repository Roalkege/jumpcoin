# jumpcoin
Community Cryptocurrency from Jumperbillijumper on YouTube

Technical specifications

- Coin Name: Jumpcoin

- Ticker: JUMP

- Premine: 0%

- Max coin supply: No max supply but TX fee burn

- Block reward: 2 Jump - 0,4 Jump Dev Donation

- Block time: 60 Seconds

- Algo: Scrypt; Pure POS Coin now

- RPC Port: 31240

- P2P Port: 31242

## Docker

This repository builds an all-in-one container image that runs the
Jumpcoin Qt wallet in a headless X server and exposes the GUI through
noVNC in any modern browser (port 6080).

### Run locally

```sh
docker compose up -d
```

Open <http://localhost:6080/vnc.html> for the Qt wallet.  See
`docker/README.md` for environment variables, ports, and the optional
`peers.dat` mount.

### Pull a pre-built image

Tagged releases are published to GitHub Container Registry:

```sh
docker pull ghcr.io/roalkege/jumpcoin-qt:v1.0.0
```

### Build a release

This repository ships a GitHub Actions workflow
(`.github/workflows/docker.yml`) that builds and pushes the image to
`ghcr.io/<owner>/jumpcoin-qt` whenever you push a tag that starts with
`v`:

```sh
git tag v1.0.0
git push origin v1.0.0
```

Tags are produced automatically (`v1.0.0`, `v1.0`, `v1`, plus a
short-SHA build) using `docker/metadata-action`.  Layers are cached in
the registry under the `buildcache` tag so subsequent builds reuse
~80% of the previous image.

## Unraid

These instructions assume Unraid 6.11+ with the `docker` tab enabled.
The image is pulled from GHCR; you don't need to build anything on the
server.

### 1. Create the appdata folder

Pick a single directory that will hold the blockchain, wallet, and
config.  Anything under `/mnt/user/` is safe (survives array
rebuilds, is replicated by the cache pool if you have one).

```sh
mkdir -p /mnt/user/appdata/jumpcoin
```

Optional — if you have a pre-existing `peers.dat` (e.g. to bootstrap
the wallet when DNS seeds are dead) drop it next to the folder:

```sh
scp peers.dat root@<unraid>:/mnt/user/appdata/jumpcoin/peers.dat
```

### 2. Add the container

**Web-GUI method** (recommended for one-off setups):

1. Open the Unraid WebUI → **Docker** → **Add Container**.
2. Fill in the fields:

   | Field | Value |
   |-------|-------|
   | Name | `jumpcoin-qt` |
   | Repository | `ghcr.io/roalkege/jumpcoin-qt` |
   | Tag | `v1.0.0` (or `latest` if you want auto-updates) |
   | Network Type | `bridge` |
   | Console shell command | `bash` (only needed for debugging) |

3. **Add the following port mappings** (click *Add another* for each):

   | Host port | Container port | Protocol |
   |-----------|----------------|----------|
   | `6080`    | `6080`         | TCP      |
   | `5900`    | `5900`         | TCP      |
   | `31240`   | `31240`        | TCP      |
   | `31242`   | `31242`        | TCP      |

   The RPC and P2P ports default to `0.0.0.0` on Unraid.  If you want
   them reachable only from other containers on the same bridge,
   remove those rows entirely — `127.0.0.1:31240:31240` from the
   compose file only works on Linux hosts with a real loopback, not
   inside Unraid's bridge networking.

4. **Add a single path mapping** (binds your appdata folder to the
   container's data directory — this is what makes the blockchain and
   wallet persist across container restarts):

   | Container path                  | Host path                                | Access |
   |---------------------------------|------------------------------------------|--------|
   | `/home/jumpcoin/.jumpcoin`      | `/mnt/user/appdata/jumpcoin/.jumpcoin`   | `rw`   |

5. **Optional: peers.dat mapping** (only if you have a bootstrap file):

   | Container path                              | Host path                                            | Access |
   |---------------------------------------------|------------------------------------------------------|--------|
   | `/home/jumpcoin/.jumpcoin/peers.dat`        | `/mnt/user/appdata/jumpcoin/peers.dat`               | `ro`   |

   If this row is set, you must place `peers.dat` on the host first
   (step 1) and **mark the container path as a file, not a
   directory** — Unraid otherwise creates an empty folder and the
   wallet silently ignores the bind.

6. **Add the following environment variables** (click *Add another*
   for each):

   | Variable | Value |
   |----------|-------|
   | `RPC_USER` | `jumpcoin` |
   | `RPC_PASSWORD` | a strong password — **change this from the default** |
   | `RPC_PORT` | `31240` |
   | `P2P_PORT` | `31242` |
   | `WALLET_RPCALLOWIP` | `0.0.0.0/0` (or `127.0.0.1` if you only use it from inside the container) |
   | `VNC_PASSWORD` | a strong password (otherwise the noVNC web UI is open to anyone on your LAN) |
   | `DISPLAY_WIDTH` | `1280` |
   | `DISPLAY_HEIGHT` | `800` |

7. **Container size** → set **Memory limit** to `4096` MB and
   **CPU weight** to `200` (or whatever fits your box).

8. **Advanced View** → set **Shared memory** to `1g` (Qt's QSharedMemory
   transport requires more than the docker default of 64 MB; without
   this, the wallet may crash on startup with a confusing error).

9. Click **Apply**.  The container pulls the image from GHCR and starts.

### 3. Open the wallet

From any machine on the same network as your Unraid box:

```
http://<unraid-ip>:6080/vnc.html
```

You should see the Jumpcoin Qt wallet boot.  If it shows
"Cannot obtain a lock on data directory", something else is using the
wallet — stop any other container or shell that points at the same
appdata folder, then restart from the Docker tab.

### 4. Upgrades

When a new tag is published (e.g. `v1.1.0`):

1. Docker tab → click the jumpcoin-qt container → **Stop**.
2. **Edit** the container, change the **Tag** field to `v1.1.0`.
3. Click **Apply** (this re-pulls the image).
4. Click **Start** once the new image is pulled.

Your appdata folder is **never touched** by an upgrade — the same
`/mnt/user/appdata/jumpcoin/.jumpcoin` keeps the same blockchain, the
same wallet, the same `jumpcoin.conf`.  If a future image needs a
config migration, follow the upgrade notes in the release.

### 5. Uninstall / data wipe

Removing the container **does not** delete the appdata folder.  To
wipe everything (e.g. resync from scratch):

```sh
rm -rf /mnt/user/appdata/jumpcoin/.jumpcoin
```

The next start of the container will recreate the folder, generate a
fresh `jumpcoin.conf`, and resync the blockchain from zero.

