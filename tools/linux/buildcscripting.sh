#!/bin/sh

PLATFORM="PLATFORM_LINUX"
DIR=$(dirname $0)
source $DIR/setupBuildEnvironment.sh $1 editorlinux

start=$(date +%s)
echo "Start time ${start}"
g++ -fpic -shared -rdynamic -nostartfiles -nostdlib  ../../Envari/source/Scripting/CScripting.cpp -o cscripting.so \
    $commonCompilerFlags
end=$(date +%s)
echo "End time ${end} $((${end}-${start}))s"

popd
popd