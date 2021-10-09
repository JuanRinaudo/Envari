@echo off

if not exist build mkdir build
pushd build
if not exist editorhtml5 mkdir editorhtml5
pushd editorhtml5

popd
popd

robocopy buildassets\html5 build\editorhtml5 /NFL /NDL /NJH /NJS

pushd build
pushd editorhtml5
node simple-server.js
popd
popd