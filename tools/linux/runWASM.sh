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

bash Envari/tools/linux/packageWASM.sh build/html5/

xdg-open http://localhost:7777/