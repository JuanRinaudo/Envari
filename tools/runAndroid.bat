@echo off

set dataPath=data

if not exist %dataPath% mkdir %dataPath%
start /D "%dataPath%" build/windows/EnvariWindows.exe