@echo off

pushd data
START /D %CD% ..\build/windows64/RuntimeWindows.exe
popd