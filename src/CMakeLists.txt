file(GLOB DISPATCHER_SOURCE ../srcDispatch/*.cpp)
file(GLOB DISPATCHER_HEADER ../srcDispatch/*.hpp)

file(GLOB SLICE_SOURCE cpp/*.cpp)
file(GLOB SLICE_HEADER headers/*.hpp)

add_executable(srcslice ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SLICE_SOURCE} ${SLICE_HEADER})
target_link_libraries(srcslice srcdispatch srcsax_static srcml ${LIBXML2_LIBRARIES})