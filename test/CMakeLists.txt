file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SRCSLICE_SOURCE
    ../src/cpp/srcslicepolicy.cpp
    ../src/headers/srcslicepolicy.hpp
    )
   
file(GLOB SRCSLICETEST_SOURCE
        srcSliceTest.cpp
        srcSliceTest.hpp
    )

find_package(srcML REQUIRED)

link_directories(/opt/homebrew/lib/)

add_executable(testPointer ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testPointer.cpp)
target_link_libraries(testPointer gtest_main gtest srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testConditional ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testConditional.cpp)
target_link_libraries(testConditional gtest_main gtest srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testPassByReference ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testPassByReference.cpp)
target_link_libraries(testPassByReference gtest_main gtest srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testPassByValue ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testPassByValue.cpp)
target_link_libraries(testPassByValue gtest_main gtest srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testConstReference ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testConstReference.cpp)
target_link_libraries(testConstReference gtest_main gtest srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)
