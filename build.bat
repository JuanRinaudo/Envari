@echo off

set CommonCompilerFlags=-nologo -Gm- -GR- -EHa- -Oi /fp:fast -WX -W4 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -DIMGUI_IMPL_OPENGL_LOADER_GL3W=1 -DGAME_SLOW=1 -DGAME_INTERNAL=1 -DGAME_WIN32=1 -DLUA_BUILD_AS_DLL=1 -Z7 -FC
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib opengl32.lib glfw3.lib glfw3dll.lib lua51.lib luajit.lib

if not exist build mkdir build
pushd build
if not exist windows mkdir windows
pushd windows
REM robocopy ..\..\Assets\Windows Assets /MIR /NFL /NDL /NJH /NJS
REM robocopy ..\..\Libraries\bin\x64 ..\windows /NFL /NDL /NJH /NJS

del /F *.pdb >NUL 2>NUL

REM 32bit build
REM cl %CommonCompilerFlags% ..\..\Source\game.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%
REM cl %CommonCompilerFlags% ..\..\Source\win32_game.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64bit build
REM Optimization Switches /O2
REM echo WAITING FOR PDB > lock.tmp
cl -MD %CommonCompilerFlags% ..\..\Envari\EnvariWindows.cpp -FmEnvari.map /EHsc /I ..\..\GLFW\include /I ..\..\LUAJIT\include /std:c++17 /link /LIBPATH:"C:\Users\Juan\Desktop\Development\Envari\GLFW\x32\lib-vc2019" /LIBPATH:"C:\Users\Juan\Desktop\Development\Envari\LUAJIT\lib\x86" %CommonLinkerFlags% /PDB:Envari_%random%.pdb
REM cl %CommonCompilerFlags% ..\..\Source\game.cpp -FmEnvari.map /LD /link /PDB:Envari_%random%.pdb /DLL -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
REM del lock.tmp
REM cl %CommonCompilerFlags% ..\..\Source\win32_game.cpp -Fmwin32_game.map /link %CommonLinkerFlags%

popd
popd