file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SLICE_SOURCE *.cpp)

add_executable(testsrcslice ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SLICE_SOURCE})

target_link_libraries(testsrcslice gtest_main srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)