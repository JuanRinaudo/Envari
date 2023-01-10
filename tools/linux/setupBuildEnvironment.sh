#!/bin/sh

if [ ! -d build ]
then
mkdir build
fi
pushd build

if [ ! -d "$2" ]
then
mkdir "$2"
fi
pushd "$2"

if [ ! -d data ]
then
ln -s ../../data data
fi
if [ ! -f lualib54.so ]
then
cp ../../Envari/LUA/linux/liblua54.so liblua54.so
fi
if [ ! -f runLinux.sh ]
then
cp ../../buildassets/linux/runLinux.sh runLinux.sh
fi

commonCompilerFlags="
    -std=c++17 
    -fpermissive 
    -I../../Envari/source/Engine 
    -I../../Envari/source/Defines 
    -I../../Envari/source/Scripting  
    -I../../Envari/LUA/include 
    -I../../Envari/ZSTD 
    -L../../Envari/LUA/linux 
    -DLUA_ENABLED 
    -DCSCRIPTING_ENABLED 
    -Wno-int-to-pointer-cast 
    -Wl,--export-dynamic 
    -ldl -lpthread -llua54 
    -D$PLATFORM "

if [ "$2" == "editorlinux" ]
then
commonCompilerFlags="$commonCompilerFlags 
    -I../../Envari/IMGUI 
    -DPLATFORM_EDITOR "
fi

if [ "$2" == "tools" ]
then
commonCompilerFlags="$commonCompilerFlags 
    -DPLATFORM_TOOLS "
else
commonCompilerFlags="$commonCompilerFlags 
    -I../../Envari/SDL2/include 
    -I../../Envari/STB 
    -I../../Envari/GL3W 
    -I../../Envari/Miniaudio 
    -I../../Envari/Engine "
fi

if [ "$1" == "release" ]
then
commonCompilerFlags="$commonCompilerFlags 
    -O2 
    -DGAME_RELEASE "
else
commonCompilerFlags="$commonCompilerFlags 
    -g "
fi

bindingsCompilerFlags=$commonCompilerFlags

engineCompilerFlags="$commonCompilerFlags 
    -I../../Envari/source/Tools 
    -I../../Envari/source/Default 
    -L../../Envari/SDL2/linux 
    -lSDL2 -lSDL2main"