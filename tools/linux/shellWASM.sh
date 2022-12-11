#!/bin/sh

echo Setup ended

if ! command -v em++ &> /dev/null
then
    if ! command -v emvsdk_env &> /dev/null
    then
        source emvsdk_env
    else
        echo "ERROR: em++ could not be found, make sure to install emscripten, also run emvsdk_env on .bashrc or add emvsdk_env to path"
    fi
fi