@echo off

if not exist build mkdir build
pushd build
if not exist html5 mkdir html5
pushd html5

popd
popd

robocopy buildassets\html5 build\html5 /NFL /NDL /NJH /NJS

pushd build
pushd html5
node simple-server.js
popd
popd