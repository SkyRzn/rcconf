#!/usr/bin/env bash

rm -rf build-test &&
mkdir build-test &&

pushd build-test &&
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=ON &&
make
popd

rm -rf test-coverage &&
mkdir test-coverage &&
build-test/test/rcconf_test &&
lcov --capture --directory . --output-file test-coverage/coverage.info &&
genhtml test-coverage/coverage.info --output-directory test-coverage/html
