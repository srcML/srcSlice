file(GLOB DISPATCHER_SOURCE ../srcDispatch/*.cpp)
file(GLOB DISPATCHER_HEADER ../srcDispatch/*.hpp)

file(GLOB SRCSLICE_SOURCE
    ../src/headers/srcslicehandler.hpp
    )
   
file(GLOB SRCSLICETEST_SOURCE
        srcSliceTest.cpp
        srcSliceTest.hpp
    )

find_package(srcML REQUIRED)

link_directories(/opt/homebrew/lib/)

# Simple macro to reduce repetitive code
macro(add_unit_test TEST_FILE)
    get_filename_component(TEST_NAME_WITH_EXTENSION ${TEST_FILE} NAME)
    string(FIND ${TEST_NAME_WITH_EXTENSION} "." EXTENSION_BEGIN)
    string(SUBSTRING ${TEST_NAME_WITH_EXTENSION} 0 ${EXTENSION_BEGIN} TEST_NAME)

    add_executable(${TEST_NAME} ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} srcsax_static srcML::LibsrcML srcdispatch ${LIBXML2_LIBRARIES} pthread)

    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endmacro()

# ctest test items
add_unit_test(testPointer.cpp)
add_unit_test(testConditional.cpp)
add_unit_test(testPassByReference.cpp)
add_unit_test(testPassByValue.cpp)
add_unit_test(testConstReference.cpp)
