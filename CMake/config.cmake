##
#  config.cmake
#
#  Copyright (C) 2014-2015 SDML (www.sdml.info)
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

# build options
#option(BUILD_UNIT_TESTS "Build unit tests for srcSlice"    ON)
#option(BUILD_EXAMPLES   "Build example tests for srcSlice" ON)

# find needed libraries
find_package(LibXml2 REQUIRED)

# include needed includes
include_directories(${LIBXML2_INCLUDE_DIR})
add_definitions("-std=c++0x -O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
