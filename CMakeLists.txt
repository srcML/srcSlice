# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2013-2024 srcML, LLC. (www.srcML.org)

cmake_minimum_required(VERSION 3.20)
project(srcSlice)

# enable ctest
enable_testing()

# find needed libraries
find_package(LibXml2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wno-reorder -Wunused-variable -Wunused-parameter -Wnarrowing -Werror=narrowing -Wno-error=all")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${LIBXML2_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}/srcDispatch/srcSAX/src/srcsax
                    ${CMAKE_SOURCE_DIR}/srcDispatch/srcSAX/src/cpp
                    ${CMAKE_SOURCE_DIR}/srcDispatch/srcSAX/src/windows
                    external
                    srcDispatch/src/dispatcher
                    srcDispatch/src/policy_classes
                    src/headers)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(srcDispatch)

# custom make target to strictly test srcSlice test suite
add_custom_target(test_srcslice
    COMMAND ${CMAKE_CTEST_COMMAND} -L srcslice # only run ctest testcases with the srcslice label attached
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS build_srcslice_tests # ensure srcslice tests are built
)

# Download the CLI11 parser into directory named "external".
set(CMAKE_EXTERNAL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external) # Creates folder if doesn't exists.
# Name of file is needed since content of file itself are being downloaded.
file(DOWNLOAD https://github.com/CLIUtils/CLI11/releases/download/v2.3.2/CLI11.hpp ${CMAKE_EXTERNAL_SOURCE_DIR}/CLI11.hpp)
