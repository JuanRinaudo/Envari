@echo off

echo Initial setup

if not exist data (
    echo Creating data folder
    mkdir data
)

echo Create inner data file structure
echo Copy data template
robocopy Envari\template-data data /NFL /NDL /NJH /NJS /MIR
pushd data

if not exist atlas (
    echo Creating atlas folder
    mkdir atlas
)
if not exist tobinary (
    echo Creating tobinary folder
    mkdir tobinary
)
if not exist data (
    echo Creating data folder
    mkdir data
)
if not exist dump (
    echo Creating dump folder
    mkdir dump
)
if not exist fonts (
    echo Creating fonts folder
    mkdir fonts
)
if not exist images (
    echo Creating images folder
    mkdir images
)
if not exist save (
    echo Creating save folder
    mkdir save
)
if not exist scripts (
    echo Creating scripts folder
    mkdir scripts
)
if not exist shaders (
    echo Creating shaders folder
    mkdir shaders
)
if not exist sound (
    echo Creating sound folder
    mkdir sound
)
if not exist temp (
    echo Creating temp folder
    mkdir temp
)
if not exist video (
    echo Creating video folder
    mkdir video
)
popd

if not exist buildassets (
    echo Copy build assets
    robocopy Envari\template-buildassets buildassets /NFL /NDL /NJH /NJS /MIR
)

if not exist jni (
    echo Copy jni
    robocopy Envari\template-jni jni /NFL /NDL /NJH /NJS /MIR
)

if not exist android-project (
    echo Copy android template
    robocopy Envari\template-android-project android-project /NFL /NDL /NJH /NJS /MIR
)