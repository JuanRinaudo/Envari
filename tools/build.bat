@echo off

cl>nul
if "%errorlevel%" == "9009" (
    vsvars.bat x86
) || (
    echo CL found, starting build
)

set CommonCompilerFlags=-MD -EHsc -std:c++17 -nologo -Gm- -GR- -EHa- -Oi /fp:fast -WX -W4 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -DGAME_WIN32=1 -DLUA_BUILD_AS_DLL=1 -DGAME_INTERNAL -DGAME_SLOW -DLUA_SCRIPTING_ENABLED -Z7 -FC
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib opengl32.lib SDL2.lib SDL2main.lib lua54.lib

pushd Envari
pushd source
for /f "delims=" %%i in ('"forfiles /m ScriptingBindings.cpp /c "cmd /c echo @ftime" "') do set ScriptingDate=%%i
popd
popd

if not exist build mkdir build
pushd build
if not exist windows mkdir windows
pushd windows

if not exist lua54.dll copy ..\..\Envari\LUA\lib\x86\lua54.dll lua54.dll >NUL
if not exist SDL2.dll copy ..\..\Envari\SDL2\lib\x86\SDL2.dll SDL2.dll >NUL

del /F *.pdb >NUL 2>NUL

@echo Start time %time%
REM -d2cgsummary
REM -Bt REM Build Time
REM -MP multiprocesor build
REM %random% to get random number
if not "%LastScriptingDate%"=="%ScriptingDate%" cl -c ..\..\Envari\source\ScriptingBindings.cpp -FmScriptingBindings.map %CommonCompilerFlags% -Bt -I ..\..\Envari\SDL2\include -I ..\..\Envari\LUA\include
set LastScriptingDate=%ScriptingDate%

cl ..\..\Envari\source\EnvariWindows.cpp ScriptingBindings.obj -FmEnvariWindows.map %CommonCompilerFlags% -Bt -I ..\..\Envari\SDL2\include -I ..\..\Envari\LUA\include -link %CommonLinkerFlags% -LIBPATH:"..\..\Envari\SDL2\lib\x86" -LIBPATH:"..\..\Envari\LUA\lib\x86" -PDB:EnvariWindows.pdb
@echo End time %time%

REM DLL SYSTEM WITH CODE RELOAD NOT WORKING RIGHT NOW
REM Optimization Switches /O2
REM echo WAITING FOR PDB > lock.tmp
REM cl %CommonCompilerFlags% ..\..\Envari\Source\game.cpp -FmEnvari.map -LD -link -PDB:Envari_%random%.pdb -DLL -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
REM del lock.tmp

popd
popd