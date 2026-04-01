# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2013-2024 srcML, LLC. (www.srcML.org)

file(GLOB DISPATCHER_SOURCE ../srcDispatch/*.cpp)
file(GLOB DISPATCHER_HEADER ../srcDispatch/*.hpp)

file(GLOB SLICE_SOURCE cpp/*.cpp)
file(GLOB SLICE_HEADER headers/*.hpp)

if(APPLE)
    # Use rpath for the install directory
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # Specify the rpath directory (in this case, /usr/local/lib)
    list(APPEND CMAKE_INSTALL_RPATH "/usr/local/lib")
endif()

add_executable(srcslice ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SLICE_SOURCE} ${SLICE_HEADER})
target_link_libraries(srcslice srcdispatch srcsax_static ${LIBXML2_LIBRARIES})

install(TARGETS srcslice)