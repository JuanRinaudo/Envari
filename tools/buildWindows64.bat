@echo off

cl>nul
if "%errorlevel%" == "9009" (
    vsvars.bat x86
) || (
    echo CL found, starting build
)

set CommonCompilerFlags=-MD -EHsc -std:c++17 -nologo -Gm- -GR- -EHa- -Oi /bigobj /fp:fast -WX -W4 -wd4457 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -DLUA_BUILD_AS_DLL=1 -DLUA_ENABLED -DGAME_RELEASE=1 -Z7 -FC
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib opengl32.lib SDL2.lib SDL2main.lib lua54.lib

pushd Envari
pushd source
for /f "delims=" %%i in ('"forfiles /m LUAScriptingBindings.cpp /c "cmd /c echo @ftime" "') do set ScriptingDate=%%i
popd
popd

robocopy Envari\template-data\scripts\envari data\scripts\envari /MIR > NUL

if not exist build mkdir build
pushd build
if not exist windows64 mkdir windows64
pushd windows64

if not exist lua54.dll copy ..\..\Envari\LUA\lib\x64\lua54.dll lua54.dll >NUL
if not exist SDL2.dll copy ..\..\Envari\SDL2\lib\x64\SDL2.dll SDL2.dll >NUL

del /F *.pdb >NUL 2>NUL

@echo Start time %time%
if not exist LUAScriptingBindings.%ScriptingDate::=.%.tmp (
    cl -c ..\..\Envari\source\LUAScriptingBindings.cpp -FmLUAScriptingBindings.map %CommonCompilerFlags% -Bt -I ..\..\Envari\SDL2\include -I ..\..\Envari\LUA\include
    del /F *.tmp >NUL 2>NUL
    echo timestamp > LUAScriptingBindings.%ScriptingDate::=.%.tmp
)

cl ..\..\Envari\source\RuntimeWindows.cpp LUAScriptingBindings.obj ^
    -FmRuntimeWindows.map %CommonCompilerFlags% -Bt ^
    -I ..\..\Envari\SDL2\include -I ..\..\Envari\LUA\include ^
    -link %CommonLinkerFlags% ^
    -LIBPATH:"..\..\Envari\SDL2\lib\x64" -LIBPATH:"..\..\Envari\LUA\lib\x64" ^
    -PDB:RuntimeWindows.pdb
@echo End time %time%

popd
popd