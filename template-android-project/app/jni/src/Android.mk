LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := C:\Users\Juan\Desktop\Development\CodingChallengesCPP\Envari\SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := C:\Users\Juan\Desktop\Development\CodingChallengesCPP\Envari\source\RuntimeAndroid.cpp

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lEGL -lGLESv3 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
