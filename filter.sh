#!/bin/bash
set -e

./build.sh
mpiexec -np 4 ./build/filter $1 $2 $3