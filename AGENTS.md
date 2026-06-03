# Agents Guide: openGauss-server

## Compilation

### Using `build.sh` (Recommended)
The primary entry point for building the project.
`sh build.sh [options]`

**Key Options:**
- `-m [debug|release|memcheck|mini]`: Build mode (default: `release`).
- `-3rd [path]`: Absolute path to `binarylibs`.
- `-pm [opengauss|lite|finance]`: Product mode (default: `opengauss`).
- `-bs [4096|8192]`: Block size (default: `8192`).
- `--cmake`: Use CMake instead of Make.
- `-pkg`: Create an installation package.

### Manual Compilation (Make/CMake)
Requires setting environment variables: `CODE_BASE`, `BINARYLIBS`, `GAUSSHOME`, `GCC_PATH`, `CC`, `CXX`, `LD_LIBRARY_PATH`, and `PATH`.

**Make Workflow:**
1. `./configure [options] --3rd=$BINARYLIBS --prefix=$GAUSSHOME`
2. `make -sj`
3. `make install -sj`

**CMake Workflow:**
1. `mkdir cmake_build && cd cmake_build`
2. `cmake .. -DENABLE_MULTIPLE_NODES=OFF -DENABLE_THREAD_SAFETY=ON -DENABLE_READLINE=ON -DENABLE_MOT=ON`
3. `make -sj && make install -sj`

## Architecture & Conventions
- **Base**: Derived from PostgreSQL. Refer to the PostgreSQL Development wiki for general internal structures.
- **Entrypoint**: Main kernel code resides in `src/gausskernel/`.
- **Third-Party**: Heavily depends on `binarylibs`. Ensure this directory is present or specified via `-3rd`.
- **Tooling**: Developer tools are located in `src/tools/`.

## Operational Commands
- `gs_preinstall`: Prepares the environment (root).
- `gs_install`: Deploys the database (omm user).
- `gs_uninstall`: Uninstalls the database (omm user).
