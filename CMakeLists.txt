cmake_minimum_required(VERSION 3.0)

project (rcconf C)

set(VERSION_INFO_MAJOR_VERSION 1)
set(VERSION_INFO_MINOR_VERSION 0)
set(VERSION_INFO_PATCH_VERSION 1)
set(CMAKE_INSTALL_PREFIX /usr/lib)

add_definitions(-DVERSION_MAJOR=${VERSION_INFO_MAJOR_VERSION})
add_definitions(-DVERSION_MINOR=${VERSION_INFO_MINOR_VERSION})
add_definitions(-DVERSION_PATCH=${VERSION_INFO_PATCH_VERSION})
add_definitions(-DPROJECT_NAME=${PROJECT_NAME})

add_compile_options(
	"-Wall"
	"-Werror"
	"$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
	"$<$<CONFIG:RELEASE>:-O3>"
)
include_directories(".")

file(GLOB SOURCES "rcconf.c")
add_library(${PROJECT_NAME} SHARED ${SOURCES})

option(UNIT_TEST "Build with unit test coverage" OFF)

if (UNIT_TEST)
	add_definitions(-DTEST_COVERAGE=1)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	add_subdirectory(test)
endif()
