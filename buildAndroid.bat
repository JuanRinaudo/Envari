@echo off

echo "<< Check if folder exists >>"
if not exist build mkdir build
pushd build
if not exist android mkdir android
pushd android

popd
popd

set buildType=%1
set runType=%2
if [%1] == [] set buildType=debug
if [%2] == [] set runType=none

echo "Build type: %buildType%"
echo "Run type: %runType%"

REM robocopy buildassets\android build\html5 /MIR /NFL /NDL /NJH /NJS

set noCommand="true"

pushd android-project

echo "<< Start gradle build >>"
if "%buildType%"=="debug" (
    cmd /C gradle assembleDebug
    echo "<< Copy build files >>"
    robocopy app\build\outputs\apk\debug ..\build\android app-debug.apk /NFL /NDL /NJH /NJS
    set noCommand="false"
)
if "%buildType%"=="release" (
    cmd /C gradle assembleRelease
    echo "<< Copy build files >>"
    robocopy app\build\outputs\apk\release ..\build\android app-release-unsigned.apk /NFL /NDL /NJH /NJS
    set noCommand="false"
)
if "%buildType%"=="tasks" (
    cmd /C gradle tasks
)
if "%buildType%"=="check" (
    cmd /C gradle check
)

if %noCommand%=="true" (
    echo "<< ERROR: NO COMMAND FOUND >>"
) else (
    echo "<<  >>"
)

popd

echo "<< Start install or run >>"
if "%runType%"=="install" (
    if "%buildType%"=="debug" (
        adb install build/android/app-debug.apk
    )
    if "%buildType%"=="release" (
        adb install build/android/app-release-unsigned.apk
    )
)
if "%runType%"=="installRun" (
    if "%buildType%"=="debug" (
        adb install build/android/app-debug.apk
    )
    if "%buildType%"=="release" (
        adb install build/android/app-release-unsigned.apk
    )
    adb shell am start -n org.libsdl.app/org.libsdl.app.SDLActivity
)
