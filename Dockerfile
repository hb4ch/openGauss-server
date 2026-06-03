FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
ENV CCACHE_DIR=/ccache
ENV MAKEFLAGS="-j$(nproc)"

RUN apt-get update && apt-get install -y \
    build-essential cmake flex bison libreadline-dev zlib1g-dev \
    libxml2-dev libxslt1-dev libssl-dev libaio-dev libncurses5-dev \
    libedit-dev pkg-config wget curl git python3 python3-dev \
    python-is-python3 uuid-dev libnuma-dev libkrb5-dev libcgroup-dev \
    libboost-dev libzstd-dev libcurl4-openssl-dev libcjson-dev \
    libopenblas-dev libminizip-dev liblz4-dev unixodbc-dev liblapacke-dev \
    ccache \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd dbgrp && useradd -g dbgrp -m -s /bin/bash omm
WORKDIR /openGauss-server
COPY . .

# Build stub libraries and wire into binarylibs
RUN gcc -c -fPIC -o /tmp/securec_stub.o docker/securec_stub.c && \
    ar rcs /usr/local/lib/libsecurec.a /tmp/securec_stub.o && \
    mkdir -p /usr/local/include/securec && \
    cp src/include/securec.h src/include/securectype.h /usr/local/include/securec/ && \
    gcc -c -fPIC -o /tmp/libdcf_stub.o docker/libdcf_stub.c && \
    gcc -shared -o /usr/local/lib/libdcf.so /tmp/libdcf_stub.o && \
    cp docker/dcf_interface.h /usr/local/include/dcf_interface.h && \
    ln -sf /usr/local/lib/libsecurec.a binarylibs/kernel/platform/Huawei_Secure_C/comm/lib/libsecurec.a && \
    ln -sf /usr/local/include/securec/securec.h binarylibs/kernel/platform/Huawei_Secure_C/comm/include/securec.h && \
    ln -sf /usr/local/include/securec/securectype.h binarylibs/kernel/platform/Huawei_Secure_C/comm/include/securectype.h && \
    ln -sf /usr/local/include/dcf_interface.h binarylibs/kernel/component/dcf/include/dcf_interface.h && \
    ln -sf /usr/local/lib/libdcf.so binarylibs/kernel/component/dcf/lib/libdcf.so && \
    ln -sf /usr/lib/x86_64-linux-gnu/libminizip.a binarylibs/kernel/dependency/zlib1.2.12/comm/lib/libminiunz.a && \
    ln -sf /usr/include/minizip/unzip.h binarylibs/kernel/dependency/zlib1.2.12/comm/include/unzip.h && \
    ln -sf /usr/include/minizip/ioapi.h binarylibs/kernel/dependency/zlib1.2.12/comm/include/ioapi.h && \
    ln -sf zlib1.2.12 binarylibs/kernel/dependency/zlib1.2.11 && \
    mkdir -p binarylibs/component/dcf/include && \
    ln -sf /usr/local/include/dcf_interface.h binarylibs/component/dcf/include/dcf_interface.h && \
    mkdir -p binarylibs/component/dcf/lib && \
    ln -sf /usr/local/lib/libdcf.so binarylibs/component/dcf/lib/libdcf.so

RUN chmod +x build_ubuntu.sh && \
    CC="ccache gcc" CXX="ccache g++" ./build_ubuntu.sh

ENV GAUSSHOME=/openGauss-server/dest
ENV PATH=$GAUSSHOME/bin:$PATH
ENV LD_LIBRARY_PATH=$GAUSSHOME/lib:/usr/local/lib:$LD_LIBRARY_PATH
ENV PGDATA=/var/lib/opengauss/data

COPY docker/dockerfiles/7.0.0-RC2/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh && mkdir -p $PGDATA && chown -R omm:dbgrp /var/lib/opengauss
EXPOSE 5432
ENTRYPOINT ["/entrypoint.sh"]
CMD ["gaussdb"]
