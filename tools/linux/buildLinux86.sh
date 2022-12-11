#!/bin/sh

# NEED 32 BIT LIBRARIES SDL AND LUA BEFORE SUPPORTING THIS

# if [ ! -d build ]
# then
# mkdir build
# fi
# pushd build

# if [ ! -d linux86 ]
# then
# mkdir linux86
# fi
# pushd linux86

# if [ ! -d data ]
# then
# ln -s ../../data data
# fi
# if [ ! -f lualib54.so ]
# then
# cp ../../Envari/LUA/linux/liblua54.so liblua54.so
# fi
# if [ ! -f runLinux86.sh ]
# then
# cp ../../buildassets/linux/runLinux86.sh runLinux86.sh
# fi

# savedLuaFileDate=`cat LUAScriptingBindings.tmp`
# luaFileDate=$(date -r ../../Envari/source/LUAScriptingBindings.cpp "+%s")
# if [ "$savedLuaFileDate" != "$luaFileDate" ]
# then
# start=$(date +%s)
# echo "Start LUA time ${start}"
# g++ ../../Envari/source/LUAScriptingBindings.cpp -c -o LUAScriptingBindings.o\
#     -std=c++17 -m32\
#     -I../../Envari/SDL2/include\
#     -I../../Envari/LUA/include\
#     -L../../Envari/SDL2/linux\
#     -L../../Envari/LUA/linux\
#     -lSDL2 -lSDL2main -ldl -lpthread -llua54\
#     -DPLATFORM_LINUX\
#     -DLUA_ENABLED\
#     -DGAME_RELEASE
# end=$(date +%s)
# echo "End LUA time ${end}"
# echo "$luaFileDate" > LUAScriptingBindings.tmp
# fi

# start=$(date +%s)
# echo "Start time ${start}"
# g++ ../../Envari/source/RuntimeLinux.cpp LUAScriptingBindings.o -o RuntimeLinux\
#     -std=c++17 -m32\
#     -I../../Envari/SDL2/include\
#     -I../../Envari/LUA/include\
#     -L../../Envari/SDL2/linux\
#     -L../../Envari/LUA/linux\
#     -lSDL2 -lSDL2main -ldl -lpthread -llua54\
#     -DPLATFORM_LINUX\
#     -DLUA_ENABLED\
#     -DGAME_RELEASE
# end=$(date +%s)
# echo "End time ${end}"

# popd
# popd