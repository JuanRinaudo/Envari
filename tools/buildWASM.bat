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

%EMSDK_PYTHON% %EMSDK%/upstream/emscripten/tools/file_packager.py build\html5\package.data ^
    --preload data/fonts@fonts ^
    --preload data/images@images ^
    --preload data/scripts@scripts ^
    --preload data/shaders@shaders ^
    --preload data/data@data ^
    --preload data/atlas@atlas ^
    --preload data/sound@sound ^
    --js-output=build\html5\package.js

@echo Start time %time%
@REM -ftime-report ^
@REM em++ Envari\source\ScriptingBindings.cpp -IEnvari\LUA\include -std=c++17 -o build\html5\ScriptingBindings.o
em++ -IEnvari\LUA\include Envari\source\RuntimeWASM.cpp Envari\LUA\include\liblua.a ^
    -gsource-map ^
    -O2 ^
    --pre-js buildassets/html5/prejs.js ^
    --source-map-base http://localhost:8888/ ^
    -DLUA_SCRIPTING_ENABLED=1 ^
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 ^
    -s USE_SDL=2 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    -s USE_WEBGL2=1 ^
    -s FULL_ES3=0 ^
    -s WASM=1 ^
    -s NO_EXIT_RUNTIME=1 ^
    -s FORCE_FILESYSTEM=1 ^
    -std=c++17 ^
    -o build\html5\index.js ^ 
    --no-heap-copy & Envari\tools\printEndTime.bat
@REM @echo End time %time%

@REM --preload-file data/fonts@fonts ^
@REM --preload-file data/images@images ^
@REM --preload-file data/scripts@scripts ^
@REM --preload-file data/shaders@shaders ^
@REM --preload-file data/data@data ^
@REM --preload-file data/atlas@atlas ^
@REM --preload-file data/sound@sound ^

REM em++ Source\wasm_game.cpp -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -lopenal -std=c++1z -s ASSERTIONS=2 --embed-file Assets\HTML5 -o build\html5\index.js