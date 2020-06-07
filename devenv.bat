@echo off

call build.bat
pushd build
pushd windows
devenv EnvariWindows.exe
popd
popd