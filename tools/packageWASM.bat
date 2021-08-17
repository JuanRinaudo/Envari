@echo off

if "%1"=="" (set PackageOutput=build\html5\) else (set PackageOutput=%1)

%EMSDK_PYTHON% %EMSDK%\upstream\emscripten\tools\file_packager.py %PackageOutput%package.data ^
    --preload data\fonts@fonts ^
    --preload data\images@images ^
    --preload data\scripts@scripts ^
    --preload data\shaders@shaders ^
    --preload data\data@data ^
    --preload data\atlas@atlas ^
    --preload data\sound@sound ^
    --js-output=%PackageOutput%package.js