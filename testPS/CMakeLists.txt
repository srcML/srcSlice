file(GLOB DISPATCHER_SOURCE ../dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER ..dispatcher/*.hpp)

add_executable(funcproftest ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} funcProfSrcSliceTest.cpp)
target_link_libraries(funcproftest srcsax_static srcml ${LIBXML2_LIBRARIES})

add_executable(srcslicetest ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} srcSliceTest.cpp)
target_link_libraries(srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES})
