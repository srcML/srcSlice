file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB testDef testdefinitions.cpp)
add_executable(testdefinitions ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${testDef})
target_link_libraries(testdefinitions gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

file(GLOB testUse testuses.cpp)
add_executable(testuses ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${testUse})
target_link_libraries(testuses gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)