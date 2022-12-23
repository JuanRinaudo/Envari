#!/bin/sh

PLATFORM="PLATFORM_LINUX"
DIR=$(dirname $0)
source $DIR/setupBuildEnvironment.sh $1 editorlinux

savedLuaFileDate=`cat LUAScriptingBindings.tmp`
luaFileDate=$(date -r ../../Envari/source/Scripting/LUAScriptingBindings.cpp "+%s")
if [ "$savedLuaFileDate" != "$luaFileDate" ]
then
start=$(date +%s)
echo "Start LUA Bindings time ${start}"
g++ ../../Envari/source/Scripting/LUAScriptingBindings.cpp -c -o LUAScriptingBindings.o\
    $bindingsCompilerFlags
end=$(date +%s)
echo "End LUA Bindings time ${end} $((${end}-${start}))s"
echo "$luaFileDate" > LUAScriptingBindings.tmp
fi

start=$(date +%s)
echo "Start time ${start}"
g++ ../../Envari/source/Runtimes/EditorLinux.cpp LUAScriptingBindings.o -o EditorLinux\
    $engineCompilerFlags
end=$(date +%s)
echo "End time ${end} $((${end}-${start}))s"

popd
popd