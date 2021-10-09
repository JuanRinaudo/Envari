@echo off

if not exist build mkdir build
pushd build
if not exist html5 mkdir html5
pushd html5
popd
popd

call Envari\tools\packageWASM.bat build\html5\

copy buildassets\html5\index.html build\html5\index.html >NUL

start "" http://localhost:7777/