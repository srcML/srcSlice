file(GLOB SRCSLICEHANDLER_SOURCE cpp/*.cpp)
file(GLOB SRCSLICEHANDLER_SOURCE2 cpp/srcSlice*.cpp)
file(GLOB SRCSLICEHANDLER_INCLUDE headers/*.hpp)
file(GLOB SRCSLICETEST_SOURCE tests/*.cpp)
file(GLOB SRCSLICETEST_INCLUDE tests/*.hpp)

link_directories(/usr/local/lib/)

add_executable(srcSlice ${SRCSLICEHANDLER_SOURCE} ${SRCSLICEHANDLER_INCLUDE})
add_executable(srcslicetest ${SRCSLICEHANDLER_SOURCE2} ${SRCSLICEHANDLER_INCLUDE} ${SRCSLICETEST_SOURCE} ${SRCSLICETEST_INCLUDE})

target_link_libraries(srcSlice srcsax_static ${LIBXML2_LIBRARIES})
target_link_libraries(srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES})