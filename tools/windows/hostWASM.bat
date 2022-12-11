@echo off

if not exist build mkdir build
pushd build
if not exist html5 mkdir html5
pushd html5

if not exist simpleServer.js copy ..\..\buildassets\html5\simpleServer.js simpleServer.js  >NUL

node simpleServer.js

popd
popd