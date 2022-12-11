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

if [ ! -f prejs.js ]
then
cp ../../buildassets/html5/prejs.js prejs.js
fi
if [ ! -f index.html ]
then
cp ../../buildassets/html5/index.html index.html
fi
if [ ! -f debugIndex.html ]
then
cp ../../buildassets/html5/debugIndex.html debugIndex.html
fi

savedLuaFileDate=`cat LUAScriptingBindings.tmp`
luaFileDate=$(date -r ../../Envari/source/LUAScriptingBindings.cpp "+%s")
if [ "$savedLuaFileDate" != "$luaFileDate" ]
then
start=$(date +%s)
echo "Start LUA time ${start}"
em++ -I../../Envari/LUA/include ../../Envari/source/LUAScriptingBindings.cpp\
    -O2\
    -DLUA_ENABLED=1\
    -DPLATFORM_WASM=1\
    -s USE_SDL=2\
    -std=c++17\
    -c\
    -o LUAScriptingBindings.o\
    --no-heap-copy
end=$(date +%s)
echo "End LUA time ${end}"
echo "$luaFileDate" > LUAScriptingBindings.tmp
fi

start=$(date +%s)
echo "Start time ${start}"
em++ -I../../Envari/LUA/include ../../Envari/source/RuntimeWASM.cpp LUAScriptingBindings.o ../../Envari/LUA/wasmlinux/liblua.a\
    -gsource-map\
    -O3\
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
    --no-heap-copy
end=$(date +%s)
echo "End time ${end}"

popd
popd