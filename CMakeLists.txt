set(CMAKE_CXX_FLAGS "-std=c++0x -O3")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

include_directories(${LIBXML2_INCLUDE_DIR} src/headers src/cpp)

add_subdirectory(src)
