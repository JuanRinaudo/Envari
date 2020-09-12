@echo off

cl>nul
if "%errorlevel%" == "9009" (
    vsvars.bat x86
) || (
    echo CL found, starting build
)

set CommonCompilerFlags=-nologo -Gm- -GR- -EHa- -Oi /fp:fast -WX -W4 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -DIMGUI_IMPL_OPENGL_LOADER_GL3W=1 -DGAME_SLOW=1 -DGAME_INTERNAL=1 -DGAME_WIN32=1 -DLUA_BUILD_AS_DLL=1 -Z7 -FC
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib opengl32.lib SDL2.lib SDL2main.lib lua.lib

if not exist build mkdir build
pushd build
if not exist windows mkdir windows
pushd windows

if not exist lua.dll copy ..\..\Envari\LUA\lib\x86\lua.dll lua.dll >NUL
if not exist SDL2.dll copy ..\..\Envari\SDL2\lib\x86\SDL2.dll SDL2.dll >NUL

del /F *.pdb >NUL 2>NUL

@echo Start time %time%
REM /d2cgsummary
cl -MD %CommonCompilerFlags% ..\..\Envari\source\EnvariWindows.cpp -FmEnvari.map /EHsc /I ..\..\Envari\SDL2\include /I ..\..\Envari\LUA\include /std:c++17 /link /LIBPATH:"..\..\Envari\SDL2\lib\x86" /LIBPATH:"..\..\Envari\LUA\lib\x86" %CommonLinkerFlags% /PDB:Envari_%random%.pdb
@echo End time %time%

REM DLL SYSTEM WITH CODE RELOAD NOT WORKING RIGHT NOW
REM Optimization Switches /O2
REM echo WAITING FOR PDB > lock.tmp
REM cl %CommonCompilerFlags% ..\..\Envari\Source\game.cpp -FmEnvari.map /LD /link /PDB:Envari_%random%.pdb /DLL -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
REM del lock.tmp

popd
popd