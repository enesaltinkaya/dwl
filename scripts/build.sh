#!/bin/bash
set -e
export PROJECT_NAME=dwl

if [ ! -d build ]; then
  mkdir build
  cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -S . -B build
fi

cmake --build build

export BIN_DIR=$(realpath build)
export release=0

# . scripts/0-blender.sh
# . scripts/1-compileShaders.sh
# . scripts/2-zipPaks.sh

timer() {
    local label="$1"
    shift
    local start=$(date +%s%3N)
    "$@"
    local end=$(date +%s%3N)
    echo "$label took $((end - start)) ms"
}

# timer "shaders.sh" . scripts/shaders.sh
# timer "0-blender-terrain.sh" . scripts/0-blender-terrain.sh
# timer "1-blender-scene.sh" . scripts/1-blender-scene.sh
# timer "data.sh" . scripts/data.sh