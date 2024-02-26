#!/bin/bash
set -ex

./build.sh
numProc=$(nproc)
echo $(pwd)
./build/filter $1 $2 $3