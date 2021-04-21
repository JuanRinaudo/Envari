@echo off

pushd data
START /D %CD% ..\build/windows86/RuntimeWindows.exe
popd