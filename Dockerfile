FROM debian:bookworm AS cpp_builder

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
build-essential \
libcurl4-openssl-dev \
git \
pkg-config \
cmake && \
apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

WORKDIR /app
COPY . .

RUN mkdir -p ./build

WORKDIR /app/build

RUN cmake ..
RUN make

FROM debian:bookworm

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
libcurl4 \
tzdata && \
apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*


WORKDIR /root

COPY --from=cpp_builder /app/build/iec104_server_simulator .
