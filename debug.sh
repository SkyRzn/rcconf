#!/usr/bin/env bash

rm -rf build-debug &&
mkdir build-debug &&

pushd build-debug &&
cmake .. -DCMAKE_BUILD_TYPE=Debug &&
make
popd

