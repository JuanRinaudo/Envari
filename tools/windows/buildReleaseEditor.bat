@echo off

REM Admin
if not exist build/editorRelease/data >nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

if '%errorlevel%' NEQ '0' (
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
    pushd "%CD%"
    CD /D "%~dp0"
REM Admin end

cd..
cd..

if not exist build mkdir build
pushd build

robocopy editor editorRelease /MIR *.dll *.exe /XD data >NUL

if not exist build mkdir build

pushd editorRelease
if not exist tools mkdir tools
copy /y "..\..\Envari\tools\packageWASM.bat" "tools\packageWASM.bat" >NUL

robocopy "..\windows86" runtimes\windows86 *.dll *.exe /MIR >NUL
robocopy "..\windows64" runtimes\windows64 *.dll *.exe /MIR >NUL
robocopy "..\html5" runtimes\html5 *.html *.js *.wasm *.map /MIR >NUL

popd

popd

if not exist "build\editorRelease\data" mklink /d "build\editorRelease\data" "..\..\data"