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

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
include(srcslice)

include_directories(src/headers src/cpp
	src/srcSAX/src/srcsax src/srcSAX/src/cpp src/srcSAX/src/windows)

add_subdirectory(src)
add_subdirectory(src/srcSAX/src)