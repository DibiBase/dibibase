FROM gcc:11.2.0-bullseye AS builder

ARG JOBS_NUMBER=2

RUN apt-get -qq update && apt-get -qq install -y \
    python3-pip && \
    apt-get clean

RUN pip install -U pip && \
    pip install meson ninja

WORKDIR /usr/src/dibibase

COPY . .

RUN meson setup build/ && \
    meson compile -C build/ -v && \
    meson install -C build/

FROM alpine:3.14.2

RUN apk --no-cache add gcompat libstdc++ ca-certificates

COPY --from=builder /usr/local/lib/x86_64-linux-gnu/libdibibase.so /usr/local/lib/x86_64-linux-gnu/libdibibase.so
COPY --from=builder /usr/local/bin/dibibase /usr/local/bin/dibibase

EXPOSE 9042
ENV LD_LIBRARY_PATH="/usr/local/lib/x86_64-linux-gnu"
ENTRYPOINT ["/usr/local/bin/dibibase"]

LABEL Name=dibibase Version=0.1.0
