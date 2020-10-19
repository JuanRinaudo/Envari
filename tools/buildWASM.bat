@echo off

if not exist build mkdir build
pushd build
if not exist html5 mkdir html5
pushd html5
if not exist Envari mkdir Envari
pushd Envari

robocopy ..\..\..\Envari\source source /MIR > NUL

popd
popd
popd

REM To build lualib.a the compiler inside scr/Makefile needs to change from gcc to emcc

@echo Start time %time%
REM em++ Envari\source\ScriptingBindings.cpp -IEnvari\LUA\include -std=c++17 -o build\html5\ScriptingBindings.o
em++ -IEnvari\LUA\include Envari\source\RuntimeWASM.cpp Envari\LUA\liblua.a -g4 ^
    --source-map-base http://localhost:8888/ ^
    -DLUA_SCRIPTING_ENABLED=1 ^
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 ^
    -std=c++17 --preload-file data/fonts@fonts ^
    --preload-file data/images@images --preload-file data/scripts@scripts --preload-file data/shaders@shaders --preload-file data/data@data --preload-file data/atlas@atlas --preload-file data/sound@sound ^
    -o build\html5\index.js --no-heap-copy
@echo End time %time%

REM em++ Source\wasm_game.cpp -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -lopenal -std=c++1z -s ASSERTIONS=2 --embed-file Assets\HTML5 -o build\html5\index.js