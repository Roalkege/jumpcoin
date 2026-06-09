# Changelog

All notable changes to the Jumpcoin Docker image are documented in this
file.  The format follows [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

> **Image tags:** `vX.Y.Z` is the standard image (no Tor);
> `vX.Y.Z-tor` is the variant with the bundled Tor daemon.

---

## [Unreleased]

Nothing yet.

---

## [1.0.0] - 2026-06-09

First tagged release of the all-in-one Docker image.  Published to
`ghcr.io/roalkege/jumpcoin-qt` with both the standard and the Tor
image variant.

### Added

- **Multi-stage Dockerfile** that builds `jumpcoind` and `jumpcoin-qt`
  from the in-tree source on `ubuntu:22.04` (Qt 5, BDB 5.3, libzmq,
  libevent, Boost, OpenSSL).
- **Headless X stack** so the Qt wallet renders without a physical
  display: Xvfb, Openbox window manager, TigerVNC, noVNC + websockify.
- **Browser-based GUI** at `http://<host>:6080/vnc.html` (no VNC client
  required).
- **Embedded full node** in the same container: `server=1` makes the
  Qt process serve JSON-RPC on port 31240.  Replaces the standalone
  `jumpcoind` (which would have collided with Qt's data-dir lock).
- **Desktop shortcut** (`~/Desktop/jumpcoin-qt.desktop`) and an
  Openbox right-click menu entry to re-launch the wallet if it
  crashes.  Supervisord also restarts it automatically in the
  background.
- **Pre-seeded `peers.dat`** can be mounted read-only from the host
  to bootstrap on networks where the DNS seeds are dead.
- **GitHub Actions workflow** (`.github/workflows/docker.yml`) that
  builds and pushes the image to GHCR on every `v*` tag.  Uses a
  per-variant `buildcache` tag in the registry to cut rebuild time
  from ~5 min to ~2 min.
- **Unraid Community Applications support**:
  `ca_profile.xml`, `templates/jumpcoin-qt.xml` with 15 Config
  entries (4 ports, 2 paths, 3 display options, 4 credential/permission
  vars, daemon args, /dev/shm).  Category: Finance:Crypto.
- **Official Jumpcoin wallet logo** as `icon.png` (256x256, RGBA,
  the teal "J" from `src/qt/res/icons/bitcoin.png`).
- **Host bind mount** for persistence (`./data/.jumpcoin`) instead of
  an anonymous docker volume, so `tar`/`rsync` backups are trivial.
- **Legal disclaimer** at the top of the README covering warranty,
  liability, and user responsibilities (no affiliation with the
  Jumpcoin developers or the Jumperbillijumper YouTube channel).
- **Unraid manual install guide** with a step-by-step WebGUI walkthrough
  covering the critical `shm_size=1g` setting and the file-vs-directory
  gotcha for the `peers.dat` bind.
- **Tor support (`:tor` image variant)**:
  - Build-arg `WITH_TOR=true` produces an image with the Tor daemon
    pre-installed.
  - Bundled Tor runs under supervisord, listening on SOCKS5
    `127.0.0.1:9050` and control `127.0.0.1:9051`.
  - User picks the network policy via `DAEMON_ARGS`:
    - `-proxy=127.0.0.1:9050` (clearnet + onion)
    - `-proxy=127.0.0.1:9050 -onlynet=onion` (onion only)
    - `-noonion` (ignore bundled tor)

### Fixed

- Data-dir lock collision: removed the redundant `jumpcoind`
  supervisord block.  `jumpcoin-qt` is the only wallet process and
  holds the lock exclusively.
- `disablewallet=1` patch in the entrypoint: existing config files
  on upgrade get the flag injected so the Qt process always owns the
  wallet (Send/Receive/Staking tabs are visible).
- chown on read-only bind mounts (e.g. user-supplied `peers.dat`)
  no longer aborts the entrypoint.
- HEALTHCHECK now also verifies the `jumpcoin-qt` process is alive,
  not just the TCP ports.

### Notes

- The Coin source itself (Bitcoin Core 0.13.2 fork) is unchanged.
- Default credentials (`jumpcoin`/`jumpcoin`) are still placeholders
  in the compose file and the CA template; users are explicitly
  warned to change them before exposing port 31240.
- Testnet / regtest is not wired up; mainnet only.

---

## Historical (pre-Docker)

Versions before 1.0.0 refer to the **Coin source tree** that this
repository carries (`src/`).  They are not Docker image tags; they
exist here only for reference.

### [Jumpcoin 2.0] - 2018-03-19

- Commit `cd955b1` - "Jumpcoin 2.0 release"
- Switched consensus from pure PoW to pure PoS.
- Block reward: 2 JUMP (0.4 JUMP dev donation).
- 60-second block time.
- Client version `v13.2.0` (`src/clientversion.h`).

### [1.1] - 2018-03-18

- Commit `6451514` - "Rename Master to Jumpcoin 1.1"

### [1.0]

- Commit `f336ecd` - "last change new version"
- Initial public release of the Jumpcoin code base, forked from
  Bitcoin Core 0.13.2.

---

[Unreleased]: https://github.com/roalkege/jumpcoin-qt/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/roalkege/jumpcoin-qt/releases/tag/v1.0.0
[Unreleased URL placeholder]: #
[Historical URL placeholder]: #
