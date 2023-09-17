#!/bin/sh

# simple build command 

set -e #exits in case of error

if [ ! -f /.dockerenv ]; then
    ./scripts/startDocker.sh  /home/dev/src/b.sh #start docker container
else

    source ../sphinx/bin/activate #activate python sphinx virtual environment
    if [ ! -f ./build/build.ninja ]; then
        cmake -S . -B ./build -G Ninja  
    fi

    cmake --build ./build
fi



