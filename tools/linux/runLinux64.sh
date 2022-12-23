#!/bin/sh

if [ ! -d build/linux64/data ]
then
ln -s ../../data build/linux64/data
fi

cwd="$PWD"
(cd ./build/linux64 && ${cwd}/build/linux64/runLinux.sh RuntimeLinux)