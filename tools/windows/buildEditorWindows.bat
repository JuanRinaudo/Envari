@echo off

cl>nul
if "%errorlevel%" == "9009" (
    vsvars.bat x64
) || (
    echo CL found, starting build
)

REM -DGAME_RELEASE=1
set CommonCompilerFlags=-MT -EHsc -std:c++17 -nologo -Gm- -GR- -EHa- -Oi /bigobj /fp:fast -WX -W4 -wd4541 -wd4702 -wd4457 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -DLUA_BUILD_AS_DLL=1 -DPLATFORM_WINDOWS=1 -DPLATFORM_EDITOR=1 -DGAME_RELEASE=1 -DLUA_ENABLED=1 -DGAME_SLOW=1 -Z7 /Wv:18 /Zc:preprocessor -FC
REM mime.lib socket.lib
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib opengl32.lib SDL2.lib SDL2main.lib lua54.lib OptickCore.lib

pushd Envari
pushd source
for /f "delims=" %%i in ('"forfiles /m LUAScriptingBindings.cpp /c "cmd /c echo @ftime" "') do set ScriptingDate=%%i
popd
popd

robocopy Envari\template-data\scripts\envari data\scripts\envari /MIR > NUL

if not exist build mkdir build
pushd build
if not exist editor mkdir editor
pushd editor

@REM if not exist mime.dll copy ..\..\Envari\LUA\windows\lib\x64\mime.dll mime.dll >NUL
@REM if not exist socket.dll copy ..\..\Envari\LUA\windows\lib\x64\socket.dll socket.dll >NUL
if not exist lua54.dll copy ..\..\Envari\LUA\windows\lib\x64\lua54.dll lua54.dll >NUL
if not exist SDL2.dll copy ..\..\Envari\SDL2\windows\x64\SDL2.dll SDL2.dll >NUL
if not exist OptickCore.dll copy ..\..\Envari\Optick\lib\x64\release\OptickCore.dll OptickCore.dll >NUL

del /F *.pdb >NUL 2>NUL

REM -d2cgsummary
REM -Bt REM Build Time
REM -MP multiprocesor build
REM %random% to get random number
@echo Start time %time%
if not exist LUAScriptingBindings.%ScriptingDate::=.%.tmp (
    @echo Rebuilding LUA Bindings
    cl -c ..\..\Envari\source\LUAScriptingBindings.cpp -FmLUAScriptingBindings.map %CommonCompilerFlags% -Bt -I ..\..\Envari\SDL2\include -I ..\..\Envari\Optick\include -I ..\..\Envari\LUA\include
    del /F *.tmp >NUL 2>NUL
    echo timestamp > LUAScriptingBindings.%ScriptingDate::=.%.tmp
)

cl ..\..\Envari\source\EditorWindows.cpp LUAScriptingBindings.obj ^
    -FmEditorWindows.map -PDB:EditorWindows.pdb ^
    -Bt %CommonCompilerFlags% ^
    -I ..\..\Envari\SDL2\include ^
    -I ..\..\Envari\Optick\include ^
    -I ..\..\Envari\LUA\include ^
    -link %CommonLinkerFlags% ^
    -LIBPATH:"..\..\Envari\SDL2\windows\x64" ^
    -LIBPATH:"..\..\Envari\Optick\lib\x64\release" ^
    -LIBPATH:"..\..\Envari\LUA\windows\x64"
@echo End time %time%

popd
popd