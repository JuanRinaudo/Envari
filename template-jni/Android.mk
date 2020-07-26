LOCAL_PATH:= $(call my-dir) # Get the local path of the project.
include $(CLEAR_VARS) # Clear all the variables with a prefix "LOCAL_"

LOCAL_SRC_FILES:=../Envari/EnvariAndroid.cpp # Indicate the source code.
LOCAL_MODULE:= EnvariGame # The name of the binary.
#include $(BUILD_SHARED_LIBRARY) # Tell ndk-build that we want to build a native executable.
include $(BUILD_EXECUTABLE) # Tell ndk-build that we want to build a native executable.