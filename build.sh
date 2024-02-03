#!/bin/bash
set -e

cmake -S . -B build > /dev/null
cd ./build
make
cd ..