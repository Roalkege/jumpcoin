# syntax=docker/dockerfile:1.6

# ==============================================================================
#  Jumpcoin Qt-Wallet Container
#  - Builds jumpcoind + jumpcoin-qt from this source tree
#  - Runs jumpcoind, Xvfb, TigerVNC, noVNC and the Qt wallet in one container
#  - Wallet is reachable via browser (noVNC) or any VNC client
# ==============================================================================

ARG UBUNTU_VERSION=22.04

# ------------------------------------------------------------------------------
#  Stage 1: Build
# ------------------------------------------------------------------------------
FROM ubuntu:${UBUNTU_VERSION} AS builder

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        autotools-dev \
        automake \
        autoconf \
        autoconf-archive \
        libtool \
        libtool-bin \
        pkg-config \
        gettext \
        m4 \
        ca-certificates \
        git \
        curl \
        wget \
        libssl-dev \
        libboost-all-dev \
        libevent-dev \
        libqrencode-dev \
        libprotobuf-dev \
        protobuf-compiler \
        libminiupnpc-dev \
        libzmq3-dev \
        libdb++-dev \
        libdb-dev \
        qtbase5-dev \
        qttools5-dev \
        qttools5-dev-tools \
        libqt5svg5-dev \
    && rm -rf /var/lib/apt/lists/*

# Use the system BDB 5.3 (--with-incompatible-bdb).  BDB 4.8 cannot be
# built with the gcc 11+ shipped in Ubuntu 22.04 (atomic.h conflict on
# __atomic_compare_exchange).  The wallet.dat format used with the
# incompatible flag is the format that every modern Bitcoin-derived
# wallet speaks, so this is the pragmatic choice.
WORKDIR /src

# Copy the source tree
COPY . .

RUN chmod +x ./share/genbuild.sh \
    && /bin/sh ./autogen.sh \
    && ./configure \
        --prefix=/usr/local \
        --with-gui=qt5 \
        --with-incompatible-bdb \
        --disable-tests \
        --disable-bench \
        --disable-hardening \
        --disable-fuzz \
        --disable-fuzz-binary \
    && make -j"$(nproc)" \
    && make install DESTDIR=/install \
    && test -x /install/usr/local/bin/jumpcoind \
    && test -x /install/usr/local/bin/jumpcoin-qt \
    && test -x /install/usr/local/bin/jumpcoin-cli \
    && test -x /install/usr/local/bin/jumpcoin-tx \
    && strip /install/usr/local/bin/jumpcoind \
             /install/usr/local/bin/jumpcoin-qt \
             /install/usr/local/bin/jumpcoin-cli \
             /install/usr/local/bin/jumpcoin-tx

# ------------------------------------------------------------------------------
#  Stage 2: Runtime
# ------------------------------------------------------------------------------
FROM ubuntu:${UBUNTU_VERSION} AS runtime

ARG DEBIAN_FRONTEND=noninteractive
ARG NOVNC_VERSION=1.5.0
ARG WEBSOCKIFY_VERSION=0.12.0
# WITH_TOR=true bundles the Tor daemon into the image and starts it
# under supervisord.  The wallet then reaches onion peers through the
# bundled SOCKS5 proxy at 127.0.0.1:9050.  Combined with the
# DAEMON_ARGS env var the user can choose:
#   -proxy=127.0.0.1:9050                  (clearnet + onion, default)
#   -proxy=127.0.0.1:9050 -onlynet=onion   (onion-only)
#   -noonion                                (ignore the bundled tor)
ARG WITH_TOR=false

ENV HOME=/home/jumpcoin \
    DISPLAY=:99 \
    DISPLAY_WIDTH=1280 \
    DISPLAY_HEIGHT=800 \
    DISPLAY_DEPTH=24 \
    VNC_PORT=5900 \
    NOVNC_PORT=6080 \
    RPC_USER=jumpcoin \
    RPC_PASSWORD=jumpcoin \
    RPC_PORT=31240 \
    P2P_PORT=31242 \
    WALLET_RPCALLOWIP=0.0.0.0/0 \
    VNC_SECURITY_TYPES=VncAuth \
    DAEMON_ARGS= \
    TOR_SOCKS_PORT=9050 \
    TOR_CONTROL_PORT=9051

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        supervisor \
        xvfb \
        xauth \
        openbox \
        menu \
        fonts-dejavu-core \
        fonts-liberation \
        libfontconfig1 \
        libgl1-mesa-dri \
        libgl1-mesa-glx \
        libegl1-mesa \
        libdbus-1-3 \
        libxkbcommon0 \
        libxcb-cursor0 \
        libxcb-icccm4 \
        libxcb-image0 \
        libxcb-keysyms1 \
        libxcb-randr0 \
        libxcb-render-util0 \
        libxcb-shape0 \
        libxcb-sync1 \
        libxcb-xfixes0 \
        libxcb-xinerama0 \
        libxcb-xkb1 \
        libxkbcommon-x11-0 \
        libqt5core5a \
        libqt5dbus5 \
        libqt5gui5 \
        libqt5network5 \
        libqt5svg5 \
        libqt5widgets5 \
        libqt5x11extras5 \
        libqrencode4 \
        libprotobuf23 \
        libevent-2.1-7 \
        libevent-pthreads-2.1-7 \
        libssl3 \
        libdb5.3++ \
        libboost-system1.74.0 \
        libboost-filesystem1.74.0 \
        libboost-thread1.74.0 \
        libboost-chrono1.74.0 \
        libboost-atomic1.74.0 \
        libboost-program-options1.74.0 \
        libzmq5 \
        libminiupnpc17 \
        tigervnc-standalone-server \
        tigervnc-scraping-server \
        tigervnc-common \
        tigervnc-tools \
        netcat-openbsd \
        # Desktop helpers (file manager draws clickable icons on the
        # Openbox desktop; thunar would also work but is much bigger)
        pcmanfm \
        # Debug helpers (screenshotting, querying the X display, etc.)
        x11-utils \
        imagemagick \
    && rm -rf /var/lib/apt/lists/*

# Optional Tor daemon.  Only installed when the image is built with
# WITH_TOR=true (e.g. the :tor tag); the :latest tag stays slim.
RUN if [ "$WITH_TOR" = "true" ]; then \
        apt-get update && apt-get install -y --no-install-recommends \
            tor \
            torsocks \
        && rm -rf /var/lib/apt/lists/* ; \
    fi

# noVNC + websockify
RUN mkdir -p /opt/novnc /opt/websockify \
    && curl -fsSL "https://github.com/novnc/noVNC/archive/v${NOVNC_VERSION}.tar.gz" \
        | tar -xz -C /opt/novnc --strip-components=1 \
    && curl -fsSL "https://github.com/novnc/websockify/archive/v${WEBSOCKIFY_VERSION}.tar.gz" \
        | tar -xz -C /opt/websockify --strip-components=1 \
    && ln -sf /opt/websockify/websockify.py /usr/local/bin/websockify \
    && chmod +x /opt/novnc/utils/novnc_proxy \
    && chmod +x /opt/websockify/websockify.py

# Copy binaries from the build stage.
COPY --from=builder /install /install
RUN mkdir -p /usr/local/bin \
    && cp -a /install/usr/local/bin/jumpcoind \
             /install/usr/local/bin/jumpcoin-qt \
             /install/usr/local/bin/jumpcoin-cli \
             /install/usr/local/bin/jumpcoin-tx \
             /usr/local/bin/ \
    && if [ -d /install/usr/local/lib ]; then \
        find /install/usr/local/lib -maxdepth 1 \
            \( -name 'libbitcoinconsensus*' -o -name 'libunivalue*' \) \
            -exec cp -an {} /usr/local/lib/ \; ; \
       fi \
    && rm -rf /install \
    && /usr/local/bin/jumpcoind --version >/dev/null \
    && QT_QPA_PLATFORM=offscreen /usr/local/bin/jumpcoin-qt --version >/dev/null

# Container layout
RUN groupadd -g 1000 jumpcoin \
    && useradd -u 1000 -g jumpcoin -m -d /home/jumpcoin -s /bin/bash jumpcoin \
    && mkdir -p /home/jumpcoin/.jumpcoin /var/log/supervisor /var/run \
    && chown -R jumpcoin:jumpcoin /home/jumpcoin

COPY docker/entrypoint.sh    /usr/local/bin/entrypoint.sh
COPY docker/supervisord.conf /etc/supervisor/supervisord.conf
COPY docker/menu.xml         /etc/xdg/openbox/menu.xml
COPY docker/launch-jumpcoin-qt.sh /usr/local/bin/launch-jumpcoin-qt

# Tor config is only relevant in the :tor variant; the file is still
# copied unconditionally so the runtime image is identical between
# variants except for the WITH_TOR-built-in dependency.
COPY docker/torrc            /etc/tor/torrc

# /var/lib/tor is the Tor data dir (DataDirectory in torrc).  Owner
# is the debian-tor user created by the tor package; tor refuses to
# start if it can't write here.
RUN if [ "$WITH_TOR" = "true" ]; then \
        mkdir -p /var/lib/tor /var/log/tor \
        && chown -R debian-tor:debian-tor /var/lib/tor /var/log/tor ; \
    fi

RUN chmod +x /usr/local/bin/entrypoint.sh /usr/local/bin/launch-jumpcoin-qt

EXPOSE 6080 5900 31240 31242

VOLUME ["/home/jumpcoin/.jumpcoin"]

HEALTHCHECK --interval=30s --timeout=5s --start-period=60s --retries=5 \
    CMD nc -z 127.0.0.1 ${NOVNC_PORT} \
        && nc -z 127.0.0.1 ${RPC_PORT} \
        && pgrep -f jumpcoin-qt >/dev/null \
        || exit 1

WORKDIR /home/jumpcoin

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
