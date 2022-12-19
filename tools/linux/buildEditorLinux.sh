#!/bin/sh

# REM -DGAME_RELEASE=1
# set CommonCompilerFlags=-MT -EHsc -std:c++17 -nologo -Gm- -GR- -EHa- -Oi /bigobj /fp:fast -WX -W4 -wd4541 -wd4702 -wd4457 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -DLUA_BUILD_AS_DLL=1 -DPLATFORM_WINDOWS=1 -DPLATFORM_EDITOR=1 -DGAME_RELEASE=1 -DLUA_ENABLED=1 -DGAME_SLOW=1 -Z7 /Wv:18 /Zc:preprocessor -FC
# REM mime.lib socket.lib
# set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib opengl32.lib SDL2.lib SDL2main.lib lua54.lib OptickCore.lib

if [ ! -d build ]
then
mkdir build
fi
pushd build

if [ ! -d editor ]
then
mkdir editor
fi
pushd editor

if [ ! -d data ]
then
ln -s ../../data data
fi
if [ ! -f lualib54.so ]
then
cp ../../Envari/LUA/linux/liblua54.so liblua54.so
fi
if [ ! -f runEditor.sh ]
then
cp ../../buildassets/linux/runEditor.sh runEditor.sh
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
    -DPLATFORM_EDITOR\
    -DLUA_ENABLED\
    -DGAME_RELEASE
end=$(date +%s)
echo "End LUA time ${end}"
echo "$luaFileDate" > LUAScriptingBindings.tmp
fi

start=$(date +%s)
echo "Start time ${start}"
g++ ../../Envari/source/Runtimes/EditorLinux.cpp LUAScriptingBindings.o -o EditorLinux\
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
    -DPLATFORM_EDITOR\
    -DLUA_ENABLED\
    -DGAME_RELEASE
end=$(date +%s)
echo "End time ${end}"

popd
popd