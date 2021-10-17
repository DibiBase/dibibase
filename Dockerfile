FROM gcc:11.2.0-bullseye AS builder

ARG JOBS_NUMBER=2

RUN apt-get -qq update && apt-get -qq install -y \
    antlr4 \
    libantlr4-runtime-dev \
    protobuf-compiler-grpc \
    libgrpc++-dev \
    cmake \
    meson && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/local/src/dibibase

COPY . .

RUN meson setup build/ && \
    meson compile -C build/ -v && \
    meson install -C build/

ENV LD_LIBRARY_PATH="/usr/local/lib/x86_64-linux-gnu"
CMD ["/usr/local/bin/dibibase"]
EXPOSE 9042

LABEL Name=dibibase Version=0.1.0
