#!/bin/sh

if [ ! -d build ]
then
mkdir build
fi
pushd build

if [ ! -d tools ]
then
mkdir tools
fi
pushd tools

if [ ! -d codegen ]
then
mkdir codegen
fi
pushd codegen

if [ ! -f lualib54.so ]
then
cp ../../../Envari/LUA/linux/liblua54.so liblua54.so
fi
if [ ! -f runCodegen.sh ]
then
cp ../../../buildassets/linux/runCodegen.sh runCodegen.sh
fi

start=$(date +%s)
echo "Start time ${start}"
g++ ../../../Envari/source/Tools/CodeGen.cpp -o codegen\
    -std=c++17\
    -I../../../Envari/LUA/include\
    -L../../../Envari/LUA/linux\
    -ldl -lpthread -llua54\
    -DPLATFORM_LINUX\
    -DLUA_ENABLED
end=$(date +%s)
echo "End time ${end}"

popd
popd
popd

cwd="$PWD"
echo ${cwd}
(cd ./data && ${cwd}/build/tools/codegen/runCodegen.sh)