@echo off

if not exist build mkdir build
pushd build
if not exist editorhtml5 mkdir editorhtml5
pushd editorhtml5
popd
popd

call Envari\tools\packageWASM.bat build\editorhtml5\

copy buildassets\editorhtml5\index.html build\editorhtml5\index.html >NUL

start "" http://localhost:7777/