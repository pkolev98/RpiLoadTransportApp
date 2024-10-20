#!/bin/bash

rm -rf build
cmake -S . -B build
cd build
make -j4
cd -

