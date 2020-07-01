APP_ABI := armeabi-v7a arm64-v8a # Define the target architecture to be ARM.
APP_OPTIM := debug    # Build the target in debug mode. 
#APP_STL := c++_shared # We use c++_shared as the standard C/C++ library.
APP_STL := c++_static # We use c++_static as the standard C/C++ library.
APP_CPPFLAGS := -frtti -fexceptions # This is the place you enable exception.
APP_PLATFORM := android-19 # Define the target Android version of the native application.