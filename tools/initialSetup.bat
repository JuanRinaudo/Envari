@echo off

echo Initial setup

if not exist data (
    echo Creating data folder
    mkdir data
)

echo Create inner data file structure
pushd data
if not exist data (
    echo Copy data template
    robocopy ..\Envari\template-data data /S /NFL /NDL /NJH /NJS
)
if not exist fonts (
    echo Creating fonts folder
    mkdir fonts
)
if not exist images (
    echo Creating images folder
    mkdir images
)
if not exist scripts (
    echo Creating scripts folder
    mkdir scripts
)
if not exist shaders (
    echo Creating shaders folder
    robocopy ..\Envari\template-shaders shaders /S /NFL /NDL /NJH /NJS
)
popd

if not exist buildassets (
    echo Copy build assets
    robocopy Envari\template-buildassets buildassets /S /NFL /NDL /NJH /NJS
)

if not exist jni (
    echo Copy jni
    robocopy Envari\template-jni jni /S /NFL /NDL /NJH /NJS
)

if not exist android-project (
    echo Copy android template
    robocopy Envari\template-android-project android-project /S /NFL /NDL /NJH /NJS
)

pause