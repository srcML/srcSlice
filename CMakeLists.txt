set(CMAKE_CXX_FLAGS "-std=c++0x")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

include_directories(${LIBXML2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/srcSAX/src/srcsax ${CMAKE_SOURCE_DIR}/srcSAX/src/cpp src/headers)

add_subdirectory(src)