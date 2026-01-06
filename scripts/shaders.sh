#!/bin/bash

# if there no shaders directories, exit
if ls data/pak_*/shaders 1> /dev/null 2>&1; then
    :
else
    return 0
fi

doFullCompile=0

function compile () {
    includeDir=$(echo $2 | cut -d'/' -f1-3)/shaders/includes
    if [[ $RELEASE == 1 ]]; then
        echo "compiling optimized $3"
        glslc -O $2 -o $3 -I ${includeDir}
    else
        echo "compiling $3"
        glslc $2 -o $3 -I ${includeDir}
    fi
}

function checkShader () {
    shaderPath=$1
    fileName="$(basename $shaderPath)"

    if [[ $RELEASE == 1 ]]; then
        spvDir="$(dirname $shaderPath)/spv"
        spvFileName="$fileName.spv.release"
    else
        spvDir="$(dirname $shaderPath)/spv"
        spvFileName="$fileName.spv.debug"
    fi
    spvFullPath="$spvDir/$spvFileName"
    mkdir -p $spvDir

    if [ ! -f $spvFullPath ]; then
        compile "$fileName" "$shaderPath" "$spvFullPath"
        return
    fi

    sourceLastModified=$(date -r $shaderPath +%s)
    spvLastModified=$(date -r $spvFullPath +%s)

    if [ "$sourceLastModified" -gt "$spvLastModified" ] || [ $doFullCompile == 1 ]; then
        compile "$fileName" "$shaderPath" "$spvFullPath"
    fi
}

md5=($(tar -cf - data/pak_*/shaders/includes | md5sum))
tmpFile="scripts/.tmp/includeShadersMD5"
if [ ! -f "$tmpFile" ]; then
    doFullCompile=1
    touch $tmpFile
    echo $md5 > $tmpFile
else
    savedMD5=$(cat $tmpFile | xargs)
    if [ "$savedMD5" != "$md5" ]; then
        doFullCompile=1
        echo $md5 > $tmpFile
    fi
fi

readarray -t vertexShaders <<< $(find ./data/pak_*/shaders -name "*.vert")
for i in "${vertexShaders[@]}"
do
    checkShader "$i"
done

readarray -t fragShaders <<< $(find ./data/pak_*/shaders -name "*.frag")
for i in "${fragShaders[@]}"
do
    checkShader "$i"
done

readarray -t tese <<< $(find ./data/pak_*/shaders -name "*.tese")
for i in "${tese[@]}"
do
    checkShader "$i"
done

readarray -t tesc <<< $(find ./data/pak_*/shaders -name "*.tesc")
for i in "${tesc[@]}"
do
    checkShader "$i"
done