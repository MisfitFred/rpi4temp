#!/bin/sh

# Clean build command 

set -e #exits in case of error

if [ ! -f /.dockerenv ]; then
    ./scripts/startDocker.sh /home/dev/src/cba.sh #start docker container
else

    source ../sphinx/bin/activate #activate python sphinx virtual environment
    rm -Rf ./build #remove build directory
    
    cmake -S . -B ./build -G Ninja  
    cmake -DTARGET_BUILD=OFF -DDOC_GENERATION=OFF -DUNIT_TEST=ON -S . -B ./utest -G Ninja

    cmake --build ./build
    cmake --build ./build --target Sphinx #build sphinx documentation
    #cmake --build ./utest
    #cmake --build ./utest --target test #run unit tests
fi



