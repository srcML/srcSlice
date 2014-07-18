set(CMAKE_CXX_FLAGS "-std=c++0x")

include_directories(src/headers ${CMAKE_SOURCE_DIR}/srcSAX/src/srcsax ${CMAKE_SOURCE_DIR}/srcSAX/src/cpp)

add_subdirectory(src)