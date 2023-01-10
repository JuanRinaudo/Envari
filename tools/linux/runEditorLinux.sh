#!/bin/sh

if [ ! -d build/editorlinux/data ]
then
ln -s ../../data build/editorlinux/data
fi

DIR=$(dirname $0)
source $DIR/buildcscripting.sh $1 editorlinux

cwd="$PWD"
(cd ./build/editorlinux && ${cwd}/build/editorlinux/runLinux.sh EditorLinux)