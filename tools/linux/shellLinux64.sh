#!/bin/sh

echo Setup ended

if ! command -v g++ &> /dev/null
then
    echo "ERROR: g++ could not be found, make sure to install gcc"
fi