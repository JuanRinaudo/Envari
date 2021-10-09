em++ -IEnvari\LUA\include Envari\source\LUAScriptingBindings.cpp ^
    -O2 ^
    -DLUA_ENABLED=1 ^
    -DPLATFORM_WASM=1 ^
    -s USE_SDL=2 ^
    -std=c++17 ^
    -c ^
    -o build\html5\LUAScriptingBindings.o ^ 
    --no-heap-copy