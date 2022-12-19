@echo off

call Envari\tools\packageWASM.bat

pushd Envari
pushd source
pushd Scripting
for /f "delims=" %%i in ('"forfiles /m LUAScriptingBindings.cpp /c "cmd /c echo @ftime" "') do set ScriptingDate=%%i
popd
popd
popd

if not exist build mkdir build
pushd build
if not exist html5 mkdir html5
pushd html5
if not exist Envari mkdir Envari
pushd Envari

robocopy ..\..\..\Envari\source source /MIR > NUL
copy buildassets\html5\index.html build\html5\index.html >NUL

popd
popd
popd

REM To build lualib.a the compiler inside scr/Makefile needs to change from gcc to emcc

@echo Start time %time%

if not exist build\editorhtml5\LUAScriptingBindings.%ScriptingDate::=.%.tmp (
    @echo Rebuilding LUA Bindings
    call Envari\tools\buildEditorWASMBindings.bat
    del /F *.tmp >NUL 2>NUL
    echo timestamp > build\editorhtml5\LUAScriptingBindings.%ScriptingDate::=.%.tmp
)

em++ Envari\source\Runtimes\EditorWASM.cpp build\editorhtml5\LUAScriptingBindings.o Envari\LUA\include\wasm\liblua.a ^
    -I ..\..\Envari\SDL2\include ^
    -I ..\..\Envari\LUA\include ^
    -I ..\..\Envari\source\Engine ^
    -I ..\..\Envari\source\Defines ^
    -I ..\..\Envari\source\Tools ^
    -I ..\..\Envari\source\Default ^
    -I ..\..\Envari\STB ^
    -I ..\..\Envari\GL3W ^
    -I ..\..\Envari\IMGUI ^
    -I ..\..\Envari\Miniaudio ^
    -I ..\..\Envari\ZSTD ^
    -gsource-map ^
    -O3 ^
    --pre-js buildassets/html5/prejs.js ^
    --source-map-base http://localhost:7777/ ^
    -DLUA_ENABLED=1 ^
    -DPLATFORM_WASM=1 ^
    -DPLATFORM_EDITOR=1 ^
    -s EXPORTED_FUNCTIONS=_main,_main_loaded,_main_end ^
    -s EXPORTED_RUNTIME_METHODS=ccall,cwrap ^
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 ^
    -s USE_SDL=2 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    -s FORCE_FILESYSTEM=1 ^
    -s MIN_WEBGL_VERSION=2 ^
    -s MAX_WEBGL_VERSION=2 ^
    -s LEGACY_GL_EMULATION=1 ^
    -s WASM=1 ^
    -s NO_EXIT_RUNTIME=1 ^
    -lidbfs.js ^
    -std=c++17 ^
    -o build\editorhtml5\index.js ^ 
    --no-heap-copy & Envari\tools\printEndTime.bat
@REM @echo End time %time%