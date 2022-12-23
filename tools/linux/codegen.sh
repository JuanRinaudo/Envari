#!/bin/sh

DIR=$(dirname $0)
source $DIR/setupBuildEnvironment.sh $1 tools

start=$(date +%s)
echo "Start time ${start}"
g++ ../../Envari/source/Tools/CodeGen.cpp -o Codegen\
    $commonCompilerFlags
end=$(date +%s)
echo "End time ${end}"

popd
popd

cwd="$PWD"
echo ${cwd}
(cd ./data && ${cwd}/build/tools/runLinux.sh Codegen)