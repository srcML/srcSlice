file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SLICE_SOURCE cpp/*.cpp)
file(GLOB SLICE_HEADER headers/*.hpp)

add_executable(srcslice ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SLICE_SOURCE} ${SLICE_HEADER})
target_link_libraries(srcslice srcsaxeventdispatch srcsax_static srcml ${LIBXML2_LIBRARIES})