#!/bin/bash

WRITE_VID=
for arg in $@; do
    if [[ ${arg} = "enable-vid" ]]; then
        WRITE_VID="-DWRITE_VID_DEBUG=1"
    fi
done

rm -rf build
cmake -S . -B build "${WRITE_VID}"
cd build
make -j4
cd -

