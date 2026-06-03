FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake flex bison libreadline-dev zlib1g-dev \
    libxml2-dev libxslt1-dev libssl-dev libaio-dev libncurses5-dev \
    libedit-dev pkg-config wget curl git python3 python3-dev \
    python-is-python3 uuid-dev libnuma-dev libkrb5-dev libcgroup-dev \
    libboost-dev libzstd-dev libcurl4-openssl-dev libcjson-dev \
    libopenblas-dev libminizip-dev liblz4-dev unixodbc-dev liblapacke-dev \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd dbgrp && useradd -g dbgrp -m -s /bin/bash omm
WORKDIR /openGauss-server
COPY . .

# Build Huawei libsecurec stub
RUN cd /tmp && mkdir -p securec && cp /openGauss-server/docker/securec_stub.c securec/ && \
    cd securec && gcc -c -fPIC -o securec_stub.o securec_stub.c && \
    ar rcs libsecurec.a securec_stub.o && \
    mkdir -p /usr/local/securec/lib /usr/local/securec/include && \
    cp libsecurec.a /usr/local/securec/lib/ && \
    cp /openGauss-server/src/include/securec.h /usr/local/securec/include/ && \
    cp /openGauss-server/src/include/securectype.h /usr/local/securec/include/ && \
    rm -rf /tmp/securec

# Build DCF stub library (eliminates binarylibs dependency)
RUN cd /tmp && mkdir -p dcf && cp /openGauss-server/docker/libdcf_stub.c dcf/ && \
    cd dcf && gcc -c -fPIC -o libdcf_stub.o libdcf_stub.c && \
    gcc -shared -o libdcf.so libdcf_stub.o && \
    mkdir -p /usr/local/dcf/lib && \
    cp libdcf.so /usr/local/dcf/lib/ && \
    rm -rf /tmp/dcf
COPY docker/dcf_interface.h /usr/local/dcf/include/dcf_interface.h

# Set up binarylibs compatibility structure for Makefile paths
RUN mkdir -p /openGauss-server/binarylibs/kernel/platform/Huawei_Secure_C/comm/lib \
    && mkdir -p /openGauss-server/binarylibs/kernel/platform/Huawei_Secure_C/comm/include \
    && ln -sf /usr/local/securec/lib/libsecurec.a /openGauss-server/binarylibs/kernel/platform/Huawei_Secure_C/comm/lib/libsecurec.a \
    && ln -sf /usr/local/securec/include/securec.h /openGauss-server/binarylibs/kernel/platform/Huawei_Secure_C/comm/include/securec.h \
    && ln -sf /usr/local/securec/include/securectype.h /openGauss-server/binarylibs/kernel/platform/Huawei_Secure_C/comm/include/securectype.h \
    && mkdir -p /openGauss-server/binarylibs/component/dcf/include \
    && mkdir -p /openGauss-server/binarylibs/component/dcf/lib \
    && ln -sf /usr/local/dcf/include/dcf_interface.h /openGauss-server/binarylibs/component/dcf/include/dcf_interface.h \
    && ln -sf /usr/local/dcf/lib/libdcf.so /openGauss-server/binarylibs/component/dcf/lib/libdcf.so \
    && mkdir -p /openGauss-server/binarylibs/kernel/dependency/zlib1.2.12/comm/lib \
    && mkdir -p /openGauss-server/binarylibs/kernel/dependency/zlib1.2.12/comm/include \
    && ln -sf /usr/lib/x86_64-linux-gnu/libminizip.a /openGauss-server/binarylibs/kernel/dependency/zlib1.2.12/comm/lib/libminiunz.a \
    && ln -sf /usr/include/minizip/unzip.h /openGauss-server/binarylibs/kernel/dependency/zlib1.2.12/comm/include/unzip.h \
    && ln -sf /usr/include/minizip/ioapi.h /openGauss-server/binarylibs/kernel/dependency/zlib1.2.12/comm/include/ioapi.h \
    && mkdir -p /openGauss-server/binarylibs/kernel/dependency/xgboost/comm/include/xgboost \
    && echo '// stub' > /openGauss-server/binarylibs/kernel/dependency/xgboost/comm/include/xgboost/c_api.h \
    && ln -sf zlib1.2.12 /openGauss-server/binarylibs/kernel/dependency/zlib1.2.11

RUN chmod +x build_ubuntu.sh
RUN ./build_ubuntu.sh

ENV GAUSSHOME=/openGauss-server/dest
ENV PATH=$GAUSSHOME/bin:$PATH
ENV LD_LIBRARY_PATH=$GAUSSHOME/lib:$GAUSSHOME/../binarylibs/kernel/component/dcf/lib:$LD_LIBRARY_PATH
ENV PGDATA=/var/lib/opengauss/data

COPY docker/dockerfiles/7.0.0-RC2/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
RUN mkdir -p $PGDATA && chown -R omm:dbgrp /var/lib/opengauss
EXPOSE 5432
ENTRYPOINT ["/entrypoint.sh"]
CMD ["gaussdb"]
