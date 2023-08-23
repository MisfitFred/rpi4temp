#!/bin/sh

#exits in case of error
set -e 
rm -Rf ./build

source ../sphinx/bin/activate
cmake -S . -B ./build -G Ninja 
cd build
ninja 
ninja Sphinx
