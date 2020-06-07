@echo off

if not exist build mkdir build
pushd build
if not exist html5 mkdir html5
pushd html5

popd
popd

robocopy buildassets\html5 build\html5 /MIR /NFL /NDL /NJH /NJS

em++ Envari\EnvariWASM.cpp -s USE_GLFW=3 -DIMGUI_IMPL_OPENGL_LOADER_GLEW=1 -DGAME_SLOW=1 -DGAME_INTERNAL=1 -O0 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -std=c++1z -o build\html5\index.js

REM em++ Source\wasm_game.cpp -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -lopenal -std=c++1z -s ASSERTIONS=2 --embed-file Assets\HTML5 -o build\html5\index.js