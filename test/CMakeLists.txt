file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SRCSLICE_SOURCE
    ../src/cpp/srcslicepolicy.cpp
    ../src/headers/srcslicepolicy.hpp
    )

add_executable(testPointer ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} srcSliceTest.cpp srcSliceTest.hpp testPointer.cpp)
target_link_libraries(testPointer gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testConditional ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} srcSliceTest.cpp srcSliceTest.hpp testConditional.cpp)
target_link_libraries(testConditional gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testPassByReference ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} srcSliceTest.cpp srcSliceTest.hpp testPassByReference.cpp)
target_link_libraries(testPassByReference gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)