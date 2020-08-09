@echo off

em++ -IEnvari\LUA\include Envari\source\EnvariWASM.cpp Envari\LUA\include\liblua.a -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_GLFW=3 -DIMGUI_IMPL_OPENGL_LOADER_GLEW=1 -DGAME_SLOW=1 -DGAME_INTERNAL=0 -O0 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -std=c++1z --preload-file data/fonts@fonts --preload-file data/images@images --preload-file data/scripts@scripts --preload-file data/shaders@shaders --preload-file data/data@data --preload-file data/atlas@atlas -o build\html5\index.js

REM em++ Source\wasm_game.cpp -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -lopenal -std=c++1z -s ASSERTIONS=2 --embed-file Assets\HTML5 -o build\html5\index.js