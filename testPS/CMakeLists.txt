file(GLOB DISPATCHER_SOURCE ../dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER ..dispatcher/*.hpp)

file(GLOB TESTSOURCE *.cpp)

add_executable(srcslicetest ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${TESTSOURCE})
target_link_libraries(srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES})