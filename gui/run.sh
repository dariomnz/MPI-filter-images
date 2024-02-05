#!/bin/bash
set -e

cd gui
./build.sh
cd ..
./gui/build/raylib-test