file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)
file(GLOB SRCSLICETEST_HEADER srcslicetest.hpp)

set(TESTFILENAME "testpointer")
file(GLOB testDef ${TESTFILENAME}.cpp)
add_executable(${TESTFILENAME} ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICETEST_HEADER} ${testDef})
target_link_libraries(${TESTFILENAME} gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)

set(TESTFILENAME "testconditional")
file(GLOB testDef ${TESTFILENAME}.cpp)
add_executable(${TESTFILENAME} ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICETEST_HEADER} ${testDef})
target_link_libraries(${TESTFILENAME} gtest_main gtest srcsax_static srcml srcsaxeventdispatch ${GTEST_LIBRARIES} ${LIBXML2_LIBRARIES} pthread)