#!/bin/sh

PLATFORM="PLATFORM_WASM"
DIR=$(dirname $0)
source $DIR/setupBuildEnvironment.sh $1 html5

savedLuaFileDate=`cat LUAScriptingBindings.tmp`
luaFileDate=$(date -r ../../Envari/source/Scripting/LUAScriptingBindings.cpp "+%s")
if [ "$savedLuaFileDate" != "$luaFileDate" ]
then
start=$(date +%s)
echo "Start LUA Bindings time ${start}"
em++ ../../Envari/source/Scripting/LUAScriptingBindings.cpp\
    -I../../Envari/LUA/include\
    -I../../Envari/SDL2/include\
    -I../../Envari/source/Defines\
    -I../../Envari/source/Engine\
    -I../../Envari/STB\
    -I../../Envari/GL3W\
    -I../../Envari/Miniaudio\
    -I../../Envari/Engine\
    -DLUA_ENABLED=1\
    -DPLATFORM_WASM=1\
    -s USE_SDL=2\
    -std=c++17\
    -c\
    -o LUAScriptingBindings.o\
    --no-heap-copy\
    -O2
end=$(date +%s)
echo "End LUA Bindings time ${end}"
echo "$luaFileDate" > LUAScriptingBindings.tmp
fi

start=$(date +%s)
echo "Start time ${start}"
em++ ../../Envari/source/Runtimes/RuntimeWASM.cpp LUAScriptingBindings.o ../../Envari/LUA/wasmlinux/liblua.a\
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
    -gsource-map\
    --pre-js ../../buildassets/html5/prejs.js\
    --source-map-base http://localhost:7777/\
    -DLUA_ENABLED=1\
    -DPLATFORM_WASM=1\
    -s EXPORTED_FUNCTIONS=_main,_main_loaded,_main_end\
    -s EXPORTED_RUNTIME_METHODS=ccall,cwrap\
    -s ERROR_ON_UNDEFINED_SYMBOLS=0\
    -s USE_SDL=2\
    -s ALLOW_MEMORY_GROWTH=1\
    -s FORCE_FILESYSTEM=1\
    -s MIN_WEBGL_VERSION=2\
    -s MAX_WEBGL_VERSION=2\
    -s LEGACY_GL_EMULATION=1\
    -s WASM=1\
    -s NO_EXIT_RUNTIME=1\
    -lidbfs.js\
    -std=c++17\
    -o index.js\
    --no-heap-copy\
    -O2
end=$(date +%s)
echo "End time ${end}"

popd
popd