#!/bin/sh

# simple build command 

set -e #exits in case of error

if [ ! -f /.dockerenv ]; then
    ./scripts/startDocker.sh /home/dev/src/t.sh #start docker container
else

    source ../sphinx/bin/activate #activate python sphinx virtual environment
    if [! -f ./build/build.ninja ]; then
        cmake -DTARGET_BUILD=OFF -DDOC_GENERATION=OFF -DUNIT_TEST=ON -S . -B ./utest -G Ninja
    fi

    cmake --build ./utest
    cmake --build ./utest --target test
fi



