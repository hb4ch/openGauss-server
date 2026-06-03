FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install a comprehensive set of build tools and dependencies
# These are mapped from the original openEuler/CentOS requirements
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    flex \
    bison \
    libreadline-dev \
    zlib1g-dev \
    libxml2-dev \
    libxslt1-dev \
    libssl-dev \
    libaio-dev \
    libncurses5-dev \
    libedit-dev \
    pkg-config \
    wget \
    curl \
    git \
    python3 \
    python3-dev \
    python-is-python3 \
    uuid-dev \
    libnuma-dev \
    libkrb5-dev \
    libcgroup-dev \
    libboost-dev \
    libzstd-dev \
    libcurl4-openssl-dev \
    libcjson-dev \
    libopenblas-dev \
    libminizip-dev \
    liblz4-dev \
    unixodbc-dev \
    liblapacke-dev \
    && rm -rf /var/lib/apt/lists/*

# Create database user
RUN groupadd dbgrp && useradd -g dbgrp -m -s /bin/bash omm

# Working directory
WORKDIR /openGauss-server
COPY . .

# Download and extract binarylibs for dependencies (DCF, SSL, etc.)
RUN curl -L "https://opengauss.obs.cn-south-1.myhuaweicloud.com/3.0.0/binarylibs/openGauss-third_party_binarylibs_Centos7.6_x86_64-3.0.3.tar.gz" -o /tmp/binarylibs.tar.gz \
    && tar xzf /tmp/binarylibs.tar.gz \
    && ln -sf openGauss-third_party_binarylibs_Centos7.6_x86_64 binarylibs \
    && rm -f /tmp/binarylibs.tar.gz

# Build a stub implementation of Huawei libsecurec (needed for linking)
RUN mkdir -p /tmp/securec
COPY docker/securec_stub.c /tmp/securec/securec_stub.c
RUN cd /tmp/securec \
    && gcc -c -fPIC -o securec_stub.o securec_stub.c \
    && ar rcs libsecurec.a securec_stub.o \
    && mkdir -p /usr/local/securec/lib /usr/local/securec/include \
    && cp libsecurec.a /usr/local/securec/lib/ \
    && cp /openGauss-server/src/include/securec.h /usr/local/securec/include/ \
    && cp /openGauss-server/src/include/securectype.h /usr/local/securec/include/ \
    && rm -rf /tmp/securec

# Set up binarylibs compatibility structure with our securec stub and zlib version mapping
RUN cd /openGauss-server \
    && BLDIR="openGauss-third_party_binarylibs_Centos7.6_x86_64" \
    && mkdir -p $BLDIR/kernel/platform/Huawei_Secure_C/comm/lib \
    && ln -sf /usr/local/securec/lib/libsecurec.a $BLDIR/kernel/platform/Huawei_Secure_C/comm/lib/libsecurec.a \
    && ln -sf /usr/local/securec/include/securec.h $BLDIR/kernel/platform/Huawei_Secure_C/comm/include/securec.h \
    && ln -sf /usr/local/securec/include/securectype.h $BLDIR/kernel/platform/Huawei_Secure_C/comm/include/securectype.h \
    && ln -sf zlib1.2.12 $BLDIR/kernel/dependency/zlib1.2.11

# Make the build script executable
RUN chmod +x build_ubuntu.sh

# Execute the build
# We run this as root to ensure all install paths are created, 
# but the database will run as 'omm'
RUN ./build_ubuntu.sh

# Setup Environment
ENV GAUSSHOME=/openGauss-server/dest
ENV PATH=$GAUSSHOME/bin:$PATH
ENV LD_LIBRARY_PATH=$GAUSSHOME/lib:$LD_LIBRARY_PATH
ENV PGDATA=/var/lib/opengauss/data

# Copy entrypoint from the repo (and fix it for Ubuntu paths)
COPY docker/dockerfiles/7.0.0-RC2/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Create data directory and set ownership
RUN mkdir -p $PGDATA && chown -R omm:dbgrp /var/lib/opengauss

EXPOSE 5432

ENTRYPOINT ["/entrypoint.sh"]
CMD ["gaussdb"]
