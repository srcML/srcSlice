file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SRCSLICE_SOURCE
    ../src/cpp/srcslicehandler.cpp
    ../src/headers/srcslicehandler.hpp
    ../src/cpp/srcsliceevent.cpp
    ../src/headers/srcsliceevent.hpp
    )
   
file(GLOB SRCSLICETEST_SOURCE
        srcSliceTest.cpp
        srcSliceTest.hpp
    )

find_package(srcML REQUIRED)

link_directories(/opt/homebrew/lib/)

add_executable(testPointer ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testPointer.cpp)
target_link_libraries(testPointer srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testConditional ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testConditional.cpp)
target_link_libraries(testConditional srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testPassByReference ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testPassByReference.cpp)
target_link_libraries(testPassByReference srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testPassByValue ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testPassByValue.cpp)
target_link_libraries(testPassByValue srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

add_executable(testConstReference ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} testConstReference.cpp)
target_link_libraries(testConstReference srcsax_static srcML::LibsrcML srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)
