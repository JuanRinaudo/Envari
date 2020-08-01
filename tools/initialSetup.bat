@echo off

echo Initial setup

echo Create data folder
if not exist data mkdir data

echo Create inner data file structure
pushd data
if not exist data mkdir data
if not exist fonts mkdir fonts
if not exist images mkdir images
if not exist scripts mkdir scripts
if not exist shaders mkdir shaders
popd

echo Copy build assets
robocopy Envari\template-buildassets buildassets /S /NFL /NDL /NJH /NJS

echo Copy jni
robocopy Envari\template-jni jni /S /NFL /NDL /NJH /NJS

echo Copy android template
robocopy Envari\template-android-project android-project /S /NFL /NDL /NJH /NJS

pause