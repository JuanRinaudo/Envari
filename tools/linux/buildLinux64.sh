#!/bin/sh

if [ ! -d build ]
then
mkdir build
fi
pushd build

if [ ! -d linux64 ]
then
mkdir linux64
fi
pushd linux64

if [ ! -d data ]
then
ln -s ../../data data
fi
if [ ! -f lualib54.so ]
then
cp ../../Envari/LUA/linux/liblua54.so liblua54.so
fi
if [ ! -f runLinux64.sh ]
then
cp ../../buildassets/linux/runLinux64.sh runLinux64.sh
fi

savedLuaFileDate=`cat LUAScriptingBindings.tmp`
luaFileDate=$(date -r ../../Envari/source/Scripting/LUAScriptingBindings.cpp "+%s")
if [ "$savedLuaFileDate" != "$luaFileDate" ]
then
start=$(date +%s)
echo "Start LUA time ${start}"
g++ ../../Envari/source/Scripting/LUAScriptingBindings.cpp -c -o LUAScriptingBindings.o\
    -std=c++17\
    -I../../Envari/LUA/include\
    -I../../Envari/source/Defines\
    -I../../Envari/source/Engine\
    -I../../Envari/STB\
    -I../../Envari/Miniaudio\
    -I../../Envari/Engine\
    -L../../Envari/LUA/linux\
    -lSDL2 -lSDL2main -ldl -lpthread -llua54\
    -DPLATFORM_LINUX\
    -DLUA_ENABLED\
    -DGAME_RELEASE
end=$(date +%s)
echo "End LUA time ${end}"
echo "$luaFileDate" > LUAScriptingBindings.tmp
fi

start=$(date +%s)
echo "Start time ${start}"
g++ ../../Envari/source/Runtimes/RuntimeLinux.cpp LUAScriptingBindings.o -o RuntimeLinux\
    -std=c++17\
    -I../../Envari/SDL2/include\
    -I../../Envari/LUA/include\
    -I../../Envari/source/Engine\
    -I../../Envari/source/Defines\
    -I../../Envari/source/Tools\
    -I../../Envari/source/Default\
    -I../../Envari/STB\
    -I../../Envari/GL3W\
    -I../../Envari/IMGUI\
    -I../../Envari/Miniaudio\
    -I../../Envari/ZSTD\
    -L../../Envari/SDL2/linux\
    -L../../Envari/LUA/linux\
    -lSDL2 -lSDL2main -ldl -lpthread -llua54\
    -DPLATFORM_LINUX\
    -DLUA_ENABLED\
    -DGAME_RELEASE
end=$(date +%s)
echo "End time ${end}"

popd
popd