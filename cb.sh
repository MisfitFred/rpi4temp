#!/bin/sh

# Clean build command 

set -e #exits in case of error

if [ ! -f /.dockerenv ]; then
    ./scripts/startDocker.sh  /home/dev/src/cb.sh #start docker container
else
    echo "inside docker"
    echo "current directory: $(pwd)"
    source ../sphinx/bin/activate #activate python sphinx virtual environment
    rm -Rf ./build #remove build directory
    
    cmake -S . -B ./build -G Ninja  
    cmake --build ./build
    cmake --build ./build --target Sphinx

fi



