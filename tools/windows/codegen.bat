@echo off

cl>nul
if "%errorlevel%" == "9009" (
    vsvars.bat x64
) || (
    echo CL found, starting build
)

set CommonCompilerFlags=-nologo -Gm- -GR- -EHa- -Oi /fp:fast -WX -W4 -wd4996 -wd4018 -wd4201 -wd4100 -wd4189 -wd4505 -wd4101 -wd4456 -Z7 -FC
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib kernel32.lib shell32.lib winmm.lib lua54.lib

if not exist build mkdir build
pushd build
if not exist tools mkdir tools
pushd tools
if not exist codegen mkdir codegen
pushd codegen

del /F *.pdb >NUL 2>NUL

cl -MD %CommonCompilerFlags% ..\..\..\Envari\source\Tools\CodeGen.cpp -FmCodeGen.map -EHsc -Bt -std:c++17 -I ..\..\..\Envari\ZSTD -I ..\..\..\Envari\source\Engine -I ..\..\..\Envari\source\Defines -I ..\..\..\Envari\source\Scripting -I ..\..\..\Envari\LUA\include -link -LIBPATH:"..\..\..\Envari\LUA\windows\x86" %CommonLinkerFlags% -PDB:CodeGen_%random%.pdb

popd
popd
popd

pushd data
"..\build\tools\codegen\CodeGen.exe"
popd