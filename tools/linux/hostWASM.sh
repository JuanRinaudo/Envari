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

if [ ! -f simpleServer.js ]
then
cp ../../buildassets/html5/simpleServer.js simpleServer.js
fi

node simpleServer.js

popd
popd