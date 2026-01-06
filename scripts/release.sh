#!/usr/bin/env bash
export PROJECT_NAME=dwl
DOCKER=0
export BIN_DIR=$(realpath release)
export RELEASE=1
export MAKEFLAGS=-j32

rm -rf release
mkdir -p release/data
scripts/data.sh

function linux() {
    if [ $DOCKER == 1 ]
    then
        # runOnDocker.sh cmake -E env PROJECT_NAME=${PROJECT_NAME} cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE='-fprofile-generate=release/linux' -S . -B release/linux
        runOnDocker.sh cmake -E env PROJECT_NAME=${PROJECT_NAME} cmake -GNinja -DCMAKE_BUILD_TYPE=Release -S . -B release/linux
        runOnDocker.sh cmake --build release/linux
    else
        # cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE='-fprofile-generate=release/linux' -S . -B release/linux
        cmake -GNinja -DCMAKE_BUILD_TYPE=Release -S . -B release/linux
        cmake --build release/linux
    fi

    mv release/linux/${PROJECT_NAME} release/${PROJECT_NAME}.linux
    cp release/linux/data/*.pak ./release/data/
    rm -rf release/linux

}

function win() {
    export WIN32=1
    export _WINDOWS=1

    cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=scripts/mingw.cmake -S . -B release/win
    cmake --build release/win

    mv release/win/${PROJECT_NAME}.exe release
    rm -rf release/win

    mingw=/home/enes/tools/llvm-mingw-20250402-ucrt-ubuntu-20.04-x86_64
    cp ${mingw}/x86_64-w64-mingw32/bin/libomp.dll release/
}

start=`date +%s%N`
if [[ $1 == linux ]]; then
    linux
elif [[ $1 == win ]]; then
    win
else
    linux
    win
fi

if ls release/pak_*.pak 1> /dev/null 2>&1; then
    rm release/pak_*.pak
fi

end=`date +%s%N`



echo "---------"
printf "compiled in %'.f ms\n" $(( ($end - $start) / 1000000 ))
echo "---------"
