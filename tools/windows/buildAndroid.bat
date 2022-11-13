@echo off

echo "<< Check if folder exists >>"
if not exist build mkdir build
pushd build
if not exist android mkdir android
pushd android

popd
popd

set BUILD_TYPE=%1
set RUN_TYPE=%2
if [%1] == [] set BUILD_TYPE=debug
if [%2] == [] set RUN_TYPE=none

echo "Build type: %BUILD_TYPE%"
echo "Run type: %RUN_TYPE%"

REM robocopy buildassets\android build\html5 /MIR /NFL /NDL /NJH /NJS

set noCommand="true"

pushd android-project

echo "<< Start gradle build >>"
if "%BUILD_TYPE%"=="debug" (
    cmd /C gradle assembleDebug
    echo "<< Copy build files >>"
    robocopy app\build\outputs\apk\debug ..\build\android app-debug.apk /NFL /NDL /NJH /NJS
    set noCommand="false"
)
if "%BUILD_TYPE%"=="release" (
    cmd /C gradle assembleRelease
    echo "<< Copy build files >>"
    robocopy app\build\outputs\apk\release ..\build\android app-release-unsigned.apk /NFL /NDL /NJH /NJS
    set noCommand="false"
)
if "%BUILD_TYPE%"=="tasks" (
    cmd /C gradle tasks
)
if "%BUILD_TYPE%"=="check" (
    cmd /C gradle check
)

if %noCommand%=="true" (
    echo "<< ERROR: NO COMMAND FOUND >>"
) else (
    echo "<<  >>"
)

popd

echo "<< Start install or run >>"
if "%RUN_TYPE%"=="install" (
    if "%BUILD_TYPE%"=="debug" (
        adb install build/android/app-debug.apk
    )
    if "%BUILD_TYPE%"=="release" (
        adb install build/android/app-release-unsigned.apk
    )
)
if "%RUN_TYPE%"=="installRun" (
    if "%BUILD_TYPE%"=="debug" (
        adb install build/android/app-debug.apk
    )
    if "%BUILD_TYPE%"=="release" (
        adb install build/android/app-release-unsigned.apk
    )
    adb shell am start -n org.libsdl.app/org.libsdl.app.SDLActivity
)
