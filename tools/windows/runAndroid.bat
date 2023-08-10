@echo off

adb install -r build/android/app-debug.apk
adb shell am start -n org.libsdl.app/org.libsdl.app.SDLActivity