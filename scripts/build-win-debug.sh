#!/bin/bash
set -e
export PROJECT_NAME=c-game
export ENGINE_DEBUG=1

if [ ! -d build-win ]; then
  mkdir build-win
  cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=scripts/mingw.cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build-win
fi

cmake --build build-win

export BIN_DIR=$(realpath build-win)
export release=0

timer() {
    local label="$1"
    shift
    local start=$(date +%s%3N)
    "$@"
    local end=$(date +%s%3N)
    echo "$label took $((end - start)) ms"
}

timer "shaders.sh" . scripts/shaders.sh
timer "0-blender-terrain.sh" . scripts/0-blender-terrain.sh
timer "1-blender-scene.sh" . scripts/1-blender-scene.sh
timer "data.sh" . scripts/data.sh

build-win/c-game.exe