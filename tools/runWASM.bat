@echo off

%EMSDK_PYTHON% %EMSDK%/upstream/emscripten/tools/file_packager.py build\html5\package.data ^
    --preload data/fonts@fonts ^
    --preload data/images@images ^
    --preload data/scripts@scripts ^
    --preload data/shaders@shaders ^
    --preload data/data@data ^
    --preload data/atlas@atlas ^
    --preload data/sound@sound ^
    --js-output=build\html5\package.js

start "" http://localhost:8888/