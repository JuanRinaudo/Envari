#!/bin/sh

if [ ! -d build/editorlinux/data ]
then
ln -s ../../data build/editorlinux/data
fi

cwd="$PWD"
(cd ./build/editorlinux && ${cwd}/build/editorlinux/runLinux.sh EditorLinux)