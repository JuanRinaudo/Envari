#ifndef CONFIG_DEFINES_H
#define CONFIG_DEFINES_H

#ifndef LUA_SCRIPTING_BINDINGS_CPP
const char* CONFIG_ANDROIDCONFIG_ENVT = "data/config/androidConfig.envt";
const char* CONFIG_LINUXCONFIG_ENVT = "data/config/linuxConfig.envt";
const char* CONFIG_WASMCONFIG_ENVT = "data/config/wasmConfig.envt";
const char* CONFIG_EDITORWASMCONFIG_ENVT = "data/config/editorWasmConfig.envt";
const char* CONFIG_EDITORWINDOWSCONFIG_ENVT = "data/config/editorWindowsConfig.envt";
const char* CONFIG_EDITORLINUXCONFIG_ENVT = "data/config/editorLinuxConfig.envt";
const char* CONFIG_WINDOWSCONFIG_ENVT = "data/config/windowsConfig.envt";

const char* CONFIG_INITLUASCRIPT = "initLuaScript";
const char* CONFIG_WINDOWSIZE = "windowSize";
const char* CONFIG_BUFFERSIZE = "bufferSize";
const char* CONFIG_RENDERSCALE = "renderScale";
const char* CONFIG_WINDOWTITLE = "windowTitle";
const char* CONFIG_VSYNC = "vsync";
const char* CONFIG_FPSLIMIT = "fpsLimit";
const char* CONFIG_FPSFIXED = "fpsFixed";
#endif

#endif