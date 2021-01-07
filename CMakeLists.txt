##
#  CMakeLists.txt
#
#  Copyright (C) 2015 SDML (www.sdml.info)
#
#  This file is part of the srcSlice.
#
#  The srcSlice is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcSlice is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcSlice; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  Build configuration file

cmake_minimum_required(VERSION 2.8)
project(srcSlice)

# find needed libraries
find_package(LibXml2 REQUIRED)
find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O0 -Wno-reorder -Wunused-variable -Wunused-parameter")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${LIBXML2_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}/srcSAXEventDispatch/srcSAX/src/srcsax
                    ${CMAKE_SOURCE_DIR}/srcSAXEventDispatch/srcSAX/src/cpp
                    ${CMAKE_SOURCE_DIR}/srcSAXEventDispatch/srcSAX/src/windows 
                    srcSAXEventDispatch/src/dispatcher
                    srcSAXEventDispatch/src/policy_classes
                    src/headers)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(srcSAXEventDispatch)