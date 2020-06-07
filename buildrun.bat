@echo off

call build.bat
if not exist data mkdir data
pushd build
pushd windows
START EnvariWindows.exe
REM /D ..\..\data START Envari.exe.exe
popd
popd