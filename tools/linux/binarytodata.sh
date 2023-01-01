#!/bin/sh

PLATFORM="PLATFORM_TOOL"
DIR=$(dirname $0)
source $DIR/setupBuildEnvironment.sh $1 tools

start=$(date +%s)
echo "Start time ${start}"
g++ ../../Envari/source/Tools/BinaryToData.cpp -o BinaryToData\
    $commonCompilerFlags
end=$(date +%s)
echo "End time ${end}"

popd
popd

cwd="$PWD"
echo ${cwd}
(cd ./data && ${cwd}/build/tools/runLinux.sh BinaryToData)