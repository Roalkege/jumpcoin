# jumpcoin
Community Cryptocurrency from Jumperbillijumper on YouTube

## Disclaimer

USE AT YOUR OWN RISK.  This is an unofficial, community-maintained
container image for a community cryptocurrency.  The authors and
contributors of this repository:

- Are **not** the original Jumpcoin developers, maintainers, or
  representatives of the Jumpcoin project, the Jumperbillijumper
  YouTube channel, or any associated entity.
- Provide this software **as-is, with no warranty of any kind**,
  express or implied, including but not limited to warranties of
  merchantability, fitness for a particular purpose, or
  non-infringement.
- Accept **no responsibility or liability** for any loss, damage, or
  inconvenience arising from your use of this image, including but
  not limited to:
  - Loss of coins, wallet contents, or private keys (e.g. through
    misconfiguration, data loss, container removal, or bugs).
  - Software defects, security vulnerabilities, or data corruption
    in the wallet or the container.
  - Network or consensus issues in the Jumpcoin blockchain, including
    double-spends, chain stalls, or 51% attacks.
  - Damage to your host system, data, or other containers.
- Make **no guarantees** about the ongoing availability, security, or
  correctness of the published image or the underlying blockchain.

You are solely responsible for:

- Backing up your `wallet.dat` (and any other wallet files) to a
  secure offline location.
- Securing your RPC credentials, VNC password, and any host ports you
  expose.
- Keeping the image and your host system up to date.
- Understanding what the `jumpcoin.conf` options you set actually do.

If you do not accept these conditions, do not use this image.

## Technical specifications

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

#### Where is the data?

The blockchain, wallet, and config are persisted to **`./data/.jumpcoin`**
on the host (host bind mount, not an anonymous docker volume).  This
means:

- `docker compose down` keeps the data.
- `docker compose down -v` is no longer needed (there is no anonymous
  volume to delete) and won't touch the data.
- Backups are plain `tar`/`rsync` of that directory — see the
  *Disclaimer* above about why you must keep an offline copy of
  `wallet.dat` at all times.
- To wipe everything and resync from scratch: `rm -rf ./data`.

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

### Install via Community Applications

Once this repository has been submitted and approved, the app shows
up in the Unraid Community Applications store under
**Finance → Crypto**.  Look for **Jumpcoin-Qt**.

The template is generated from
[`templates/jumpcoin-qt.xml`](templates/jumpcoin-qt.xml) in this
repository; the Community Apps pipeline reads it on every release.

### Manual install on Unraid

If you'd rather not wait for CA approval, the image can be set up by
hand on Unraid 6.11+ with the `docker` tab enabled.  The image is
pulled from GHCR; you don't need to build anything on the server.

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

