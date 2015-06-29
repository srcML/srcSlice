file(GLOB SRCSLICEHANDLER_SOURCE cpp/*.cpp)
file(GLOB SRCSLICEHANDLER_INCLUDE headers/*.hpp)

add_executable(srcSlice ${SRCSLICEHANDLER_SOURCE} ${SRCSLICEHANDLER_INCLUDE})

target_link_libraries(srcSlice srcsax ${LIBXML2_LIBRARIES})