# RCCONF

RCCONF is a simple config parser and manager. The primary purpose is
to work with FreeBSD /etc/rc.conf, but it can work with any config
containing key="val" pairs. The parser silently skips all the incorrect
lines. You can get/set/del values by keys and save the result.

# 1. Usage
You can copy rcconf.* files to your project, include rcconf.h header and
add rcconf.c as compilation unit. That's all. If you want to build RCCONF
as a shared library or execute the tests, see the sections below.

# 2. Preparing
Install cmake.

If you want to check test coverage, install cheat unit test from
https://github.com/Tuplanolla/cheat. You should copy cheat.h and cheats.h
to /usr/include or just to the project root. Also you should install lcov
tool.

# 3 Building
Execute release.sh to build release library version. The results you can
find in build-release directory.

Execute debug.sh to build debug library version. The results you can find
in build-debug directory.

Execute test.sh to build test library version. The results you can find
in build-test directory. The results is the same as for debug version but
unit tests are enabled. The script starts tests and process the files to
collect test coverage information. Look into test-coverage directory to
get information about test coverage.
