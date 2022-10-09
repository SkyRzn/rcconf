cmake_minimum_required(VERSION 3.0)

project(${PROJECT_NAME}_test)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/rcconf.c" "test.c")

add_executable(${PROJECT_NAME} ${SOURCES})
