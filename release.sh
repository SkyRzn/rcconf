#!/usr/bin/env bash

rm -rf build-release &&
mkdir build-release &&

pushd build-release &&
cmake .. -DCMAKE_BUILD_TYPE=Release &&
make
popd

