#!/bin/sh

if [ "$*" == "" ]
then
    echo "Program name not supplied"
    exit 1
fi

CURRENT_DIR=$(dirname $0)

if [ ! -f "$CURRENT_DIR/$1" ]; then
    echo "$CURRENT_DIR/$1 does not exist."
    exit 1
fi

LD_LIBRARY_PATH=$CURRENT_DIR:$LD_LIBARY_PATH
export LD_LIBARY_PATH
exec $CURRENT_DIR/$1 "$@"