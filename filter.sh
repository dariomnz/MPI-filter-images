#!/bin/bash
set -ex

./build.sh
numProc=$(nproc)
echo $(pwd)
mpiexec -np numProc ./build/filter $1 $2 $3