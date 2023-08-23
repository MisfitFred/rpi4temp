#!/bin/sh

#exits in case of error
set -e 

rm -Rf ./utest
source ../sphinx/bin/activate
cmake -DTARGET_BUILD=OFF -DDOC_GENERATION=OFF -DUNIT_TEST=ON -S . -B ./utest -G Ninja 

cd utest
ninja 
ninja test
ninja cov