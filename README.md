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

