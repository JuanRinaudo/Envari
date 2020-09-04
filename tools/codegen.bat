@echo off

cl>nul
if "%errorlevel%" == "9009" (
    vsvars.bat x86
) || (
    echo CL found, starting build
)

set CommonCompilerFlags=-nologo -Gm- -GR- -EHa- -Oi /fp:fast -WX -W4 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -Z7 -FC
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib lua.lib

if not exist build mkdir build
pushd build
if not exist codegen mkdir codegen
pushd codegen

del /F *.pdb >NUL 2>NUL

cl -MD %CommonCompilerFlags% ..\..\Envari\source\CodeGen.cpp -FmCodeGen.map /EHsc /I ..\..\Envari\LUA\include /std:c++17 /link /LIBPATH:"..\..\Envari\LUA\lib\x86" %CommonLinkerFlags% /PDB:CodeGen_%random%.pdb

popd
popd

pushd data
"..\build\codegen\CodeGen.exe"
popd