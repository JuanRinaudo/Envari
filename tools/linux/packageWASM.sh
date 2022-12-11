#!/bin/sh

if [ ! -d build ]
then
mkdir build
fi
pushd build

if [ ! -d html5 ]
then
mkdir html5
fi
pushd html5

popd
popd

if [ -z "$1" ]
then
    packageOutput=build/html5/
else
    packageOutput=$1
fi

python ${EMSDK}/upstream/emscripten/tools/file_packager.py\
    ${packageOutput}package.data\
    --preload data/fonts@fonts\
    --preload data/images@images\
    --preload data/scripts@scripts\
    --preload data/shaders@shaders\
    --preload data/data@data\
    --preload data/atlas@atlas\
    --preload data/sound@sound\
    --js-output=${packageOutput}package.js