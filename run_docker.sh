#!/bin/bash
# Simple script to build and run openGauss in Docker

# Usage: ./run_docker.sh [BINARYLIBS_PATH]
BINARYLIBS_PATH=${1:-"/path/to/binarylibs"}

echo "Building Docker image..."
docker build --build-arg BINARYLIBS_PATH=$BINARYLIBS_PATH -t opengauss-community .

echo "Running Docker container..."
docker run -d \
  --name opengauss-dev \
  -e GS_PASSWORD="Enmo@123" \
  -e GS_USER=omm \
  -p 5432:5432 \
  opengauss-community
