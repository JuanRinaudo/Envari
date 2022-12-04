if [ ! -d build/editor/data ]
then
ln -s ../../data build/editor/data
fi

cwd="$PWD"
(cd ./build/editor && ${cwd}/build/editor/runEditor.sh)