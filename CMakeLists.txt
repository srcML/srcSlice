cmake_minimum_required(VERSION 2.8)
project(srcSlice)

find_package(LibXml2 REQUIRED)

include_directories(${LIBXML2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/../srcSAX/src/srcsax ${CMAKE_SOURCE_DIR}/../srcSAX/src/cpp src/headers)


SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

message(${CMAKE_SOURCE_DIR})

add_subdirectory(src)