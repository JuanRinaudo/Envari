@echo off

pushd data
START /D %CD% ..\build/windows/RuntimeWindows.exe
popd