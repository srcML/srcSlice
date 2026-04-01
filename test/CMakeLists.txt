# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2013-2024 srcML, LLC. (www.srcML.org)

file(GLOB DISPATCHER_SOURCE ../srcDispatch/*.cpp)
file(GLOB DISPATCHER_HEADER ../srcDispatch/*.hpp)

file(GLOB SRCSLICE_SOURCE
    ../src/headers/srcslice*.hpp
    ../src/cpp/srcslice*.cpp
    )
   
file(GLOB SRCSLICETEST_SOURCE
        srcSliceTest.cpp
        srcSliceTest.hpp
    )

find_package(srcML REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

link_directories(/opt/homebrew/lib/)

# dependency target that checks if the following testcases are built
add_custom_target(build_srcslice_tests)

# Simple macro to reduce repetitive code
macro(add_unit_test TEST_FILE)
    get_filename_component(TEST_NAME_WITH_EXTENSION ${TEST_FILE} NAME)
    string(FIND ${TEST_NAME_WITH_EXTENSION} "." EXTENSION_BEGIN)
    string(SUBSTRING ${TEST_NAME_WITH_EXTENSION} 0 ${EXTENSION_BEGIN} TEST_NAME)

    add_executable(${TEST_NAME} ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SRCSLICE_SOURCE} ${SRCSLICETEST_SOURCE} ${TEST_FILE})
    target_link_libraries(
        ${TEST_NAME}
        srcsax_static
        srcML::LibsrcML
        srcdispatch
        ${LIBXML2_LIBRARIES}
        pthread
        nlohmann_json::nlohmann_json
    )

    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    # add special label to test
    set_tests_properties(${TEST_NAME} PROPERTIES
        LABELS "srcslice"
    )
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    add_dependencies(build_srcslice_tests ${TEST_NAME})
endmacro()

# ctest test items
file(GLOB TESTS test*.cpp)
foreach(TEST IN ITEMS ${TESTS})
	add_unit_test(${TEST})
endforeach()