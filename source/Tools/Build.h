#ifndef BUILD_H
#define BUILD_H

static i32 BuildPlatform(RuntimePlatform platform)
{
    Log("Start to build platform: %s", platformNames[platform]);

    filesystem::path runtimePath = filesystem::path(editorConfig.runtimesPath->value);
    filesystem::path dataPath = filesystem::path(editorConfig.dataPath->value);
    filesystem::path outputPath = "";

    if(platform == RuntimePlatform_WINDOWS_86) {
        outputPath = filesystem::path(windows86OutputConfig.outputPath->value).concat("\\windows86");
        runtimePath += "\\windows86";
    }
    if(platform == RuntimePlatform_WINDOWS_64) {
        outputPath = filesystem::path(windows64OutputConfig.outputPath->value).concat("\\windows64");
        runtimePath += "\\windows64";
    }
    if(platform == RuntimePlatform_ANDROID) {
        LogError("Android platform is still WIP");
        return 0;
    }
    if(platform == RuntimePlatform_WASM) {
        outputPath = filesystem::path(wasmOutputConfig.outputPath->value).concat("\\html5");
        runtimePath += "\\html5";
    }

    if(outputPath != "") {
        Log("Building to: %s", outputPath.c_str());

        try {
            filesystem::copy(runtimePath, outputPath, filesystem::copy_options::update_existing | filesystem::copy_options::recursive);

            if(platform == RuntimePlatform_WINDOWS_86 || platform == RuntimePlatform_WINDOWS_64) {
                    filesystem::path outputDataPath = filesystem::path(outputPath.c_str()).concat("\\data");
                    filesystem::copy(dataPath, outputDataPath, filesystem::copy_options::update_existing | filesystem::copy_options::recursive);
            }
            else if(platform == RuntimePlatform_WASM) {
#ifdef PLATFORM_WINDOWS
                system("cmd /k \"cd.. & emsdk_env & tools\\packageWASM data\\temp\\ & exit");
#endif
                if(filesystem::exists(filesystem::path("temp\\package.js")) && filesystem::exists(filesystem::path("temp\\package.data"))) {
                    filesystem::path runtimeDataFilePath = filesystem::path(dataPath).concat("\\temp\\package.data");
                    filesystem::path outputDataFilePath = filesystem::path(outputPath).concat("\\package.data");

                    filesystem::path runtimeJsFilePath = filesystem::path(dataPath).concat("\\temp\\package.js");
                    filesystem::path outputJsFilePath = filesystem::path(outputPath).concat("\\package.js");

                    filesystem::copy(runtimeDataFilePath, outputDataFilePath, filesystem::copy_options::update_existing);
                    filesystem::copy(runtimeJsFilePath, outputJsFilePath, filesystem::copy_options::update_existing);
                }
                else {
                    LogError("Packaged data not found, check for error in the packaging step");
                    return 0;
                }
            }
        } catch(filesystem::filesystem_error& e) {
            LogError("Error trying to build: %s", e.what());
            return 0;
        }
    }

    return 1;
}

#endif