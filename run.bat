@echo off

if not exist data mkdir data
pushd build
pushd windows
REM START EnvariWindows.exe.exe
START EnvariWindows.exe /D ..\..\data
popd
popd