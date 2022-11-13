if [ ! -d build/editor/data ]
then
ln -s ../../data build/editor/data
fi

cwd="$PWD"
(cd ./build/editor/data && ${cwd}/build/editor/EditorLinux)