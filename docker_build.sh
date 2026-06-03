#!/bin/bash
# Persistent container build for openGauss.
# First run: creates dev image + compiles everything (~30-40 min)
# Subsequent runs: only recompiles changed files (fast)
#
# Usage:
#   ./docker_build.sh          # build (first time or incremental)
#   ./docker_build.sh make     # quick re-make only
#   ./docker_build.sh sql      # launch database and test SQL

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
IMAGE="opengauss-dev"
CONTAINER="og-builder"

# --- Build dev image (one-time) ---
if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
    echo ">>> Building dev image (one-time)..."
    docker build -f "$SCRIPT_DIR/Dockerfile.dev" -t "$IMAGE" "$SCRIPT_DIR"
fi

# --- Ensure persistent container ---
if ! docker inspect "$CONTAINER" >/dev/null 2>&1; then
    echo ">>> Creating persistent build container..."
    docker run -d --name "$CONTAINER" \
        -v "$SCRIPT_DIR:/openGauss-server" \
        -v "og-ccache:/ccache" \
        -e CCACHE_DIR=/ccache \
        --entrypoint sleep "$IMAGE" infinity
fi
if [ "$(docker ps -q -f name=$CONTAINER)" = "" ]; then
    docker start "$CONTAINER"
fi

# --- Fast path: just run make ---
if [ "$1" = "make" ]; then
    docker exec -w /openGauss-server "$CONTAINER" make -j$(nproc) 2>&1 | tail -20
    docker exec -w /openGauss-server "$CONTAINER" make install 2>&1 | tail -5
    exit 0
fi

# --- Full build setup ---
docker exec "$CONTAINER" bash -c '
set -e
cd /openGauss-server

# 1. Stub libraries
if [ ! -f /usr/local/lib/libsecurec.a ]; then
    gcc -c -fPIC -o /tmp/securec_stub.o docker/securec_stub.c
    ar rcs /usr/local/lib/libsecurec.a /tmp/securec_stub.o
    mkdir -p /usr/local/include/securec
    cp src/include/securec.h src/include/securectype.h /usr/local/include/securec/
fi
if [ ! -f /usr/local/lib/libdcf.so ]; then
    gcc -c -fPIC -o /tmp/libdcf_stub.o docker/libdcf_stub.c
    gcc -shared -o /usr/local/lib/libdcf.so /tmp/libdcf_stub.o
    cp docker/dcf_interface.h /usr/local/include/dcf_interface.h
fi

# 2. Wire stubs into binarylibs (idempotent)
ln -sf /usr/local/lib/libsecurec.a    binarylibs/kernel/platform/Huawei_Secure_C/comm/lib/libsecurec.a
ln -sf /usr/local/include/securec/securec.h    binarylibs/kernel/platform/Huawei_Secure_C/comm/include/securec.h
ln -sf /usr/local/include/securec/securectype.h binarylibs/kernel/platform/Huawei_Secure_C/comm/include/securectype.h
ln -sf /usr/local/include/dcf_interface.h  binarylibs/kernel/component/dcf/include/dcf_interface.h
ln -sf /usr/local/lib/libdcf.so           binarylibs/kernel/component/dcf/lib/libdcf.so
mkdir -p binarylibs/component/dcf/include binarylibs/component/dcf/lib
ln -sf /usr/local/include/dcf_interface.h  binarylibs/component/dcf/include/dcf_interface.h
ln -sf /usr/local/lib/libdcf.so           binarylibs/component/dcf/lib/libdcf.so
ln -sf /usr/lib/x86_64-linux-gnu/libminizip.a binarylibs/kernel/dependency/zlib1.2.12/comm/lib/libminiunz.a
ln -sf /usr/include/minizip/unzip.h  binarylibs/kernel/dependency/zlib1.2.12/comm/include/unzip.h
ln -sf /usr/include/minizip/ioapi.h  binarylibs/kernel/dependency/zlib1.2.12/comm/include/ioapi.h
ln -sf zlib1.2.12 binarylibs/kernel/dependency/zlib1.2.11

# 3. Pre-build gsqlerr stubs
mkdir -p src/bin/gsqlerr
cat > src/bin/gsqlerr/gsqlerr_errmsg.h << "EOF"
#ifndef ERRMSG_H
#define ERRMSG_H
#include <stdio.h>
#include <stddef.h>
#define STRING_MAX_LEN 1024
#define ERROR_LOCATION_NUM 5
typedef struct { char msg[STRING_MAX_LEN]; char cause[STRING_MAX_LEN]; char action[STRING_MAX_LEN]; } mppdb_detail_errmsg_t;
typedef struct { char szFileName[256]; unsigned int ulLineno; } mppdb_err_msg_location_t;
typedef struct { int ulSqlErrcode; char cSqlState[5]; int mppdb_err_msg_locnum; mppdb_err_msg_location_t *astErrLocate[ERROR_LOCATION_NUM]; mppdb_detail_errmsg_t stErrmsg; char ucOpFlag; } gsqlerr_err_msg_t;
static gsqlerr_err_msg_t g_gsqlerr_errors[] = {{0, "", 0, {0}}};
static gsqlerr_err_msg_t g_mppdb_errors[] = {{0, "", 0, {0}}};
#endif
EOF
cp src/bin/gsqlerr/gsqlerr_errmsg.h src/bin/gsqlerr/errmsg.h

# 4. ONNX/Tokenizers stub .a files
mkdir -p binarylibs/kernel/dependency/onnxruntime/comm/lib
mkdir -p binarylibs/kernel/dependency/tokenizers/comm/lib
gcc -c -fPIC -o /tmp/onnx_stubs.o docker/onnx_stubs.c 2>/dev/null || true
ar rcs binarylibs/kernel/dependency/onnxruntime/comm/lib/libonnx_wrapper.a /tmp/onnx_stubs.o 2>/dev/null || true
echo "void onnxruntime_stub(){}" | gcc -xc -c -fPIC -o /tmp/onnxruntime.o - 2>/dev/null || true
ar rcs binarylibs/kernel/dependency/onnxruntime/comm/lib/libonnxruntime.a /tmp/onnxruntime.o 2>/dev/null || true
echo "void tokenizers_stub(){}" | gcc -xc -c -fPIC -o /tmp/tokenizers.o - 2>/dev/null || true
ar rcs binarylibs/kernel/dependency/tokenizers/comm/lib/libtokenizers.a /tmp/tokenizers.o 2>/dev/null || true

# 5. Configure
if [ ! -f config.status ]; then
    ./configure --prefix=/openGauss-server/dest --enable-debug --enable-cassert \
        --disable-thread-safety --with-readline --enable-lite-mode \
        --disable-llvm --without-python --without-gssapi
fi

# 6. Build & install
echo ">>> Compiling (first time ~30-40 min, incremental is fast)..."
make -j$(nproc) 2>&1 | tail -10
make install 2>&1 | tail -5
echo ">>> Build complete"
'

# --- After build, test if requested ---
if [ "$1" = "sql" ] || [ "$2" = "sql" ]; then
    echo ">>> Launching database..."
    docker exec -d "$CONTAINER" bash -c "
        export GAUSSHOME=/openGauss-server/dest
        export PATH=\$GAUSSHOME/bin:\$PATH
        export LD_LIBRARY_PATH=\$GAUSSHOME/lib:/usr/local/lib:\$LD_LIBRARY_PATH
        rm -rf /tmp/pgdata && mkdir -p /tmp/pgdata
        gaussdb -D /tmp/pgdata initdb 2>&1 | tail -5
        gaussdb -D /tmp/pgdata -p 5432 &
        sleep 3
        gsql -d postgres -p 5432 -c \"CREATE TABLE test (id INT, name TEXT);\"
        gsql -d postgres -p 5432 -c \"INSERT INTO test VALUES (1, '\''hello'\'');\"
        gsql -d postgres -p 5432 -c \"SELECT * FROM test;\"
    " 2>&1
fi
