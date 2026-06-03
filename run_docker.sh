#!/bin/bash
# Usage:
#   ./run_docker.sh          # full build + run
#   ./run_docker.sh make     # quick recompile (only changed files)
#   ./run_docker.sh sql      # test with SQL after build

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Build dev image (one-time) + compile inside persistent container
exec "$SCRIPT_DIR/docker_build.sh" "$@"
