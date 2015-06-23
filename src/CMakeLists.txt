file(GLOB SRCSAXHANDLER_SOURCE ../../srcSAX/src/srcsax/*.cpp ../../srcSAX/src/cpp/*.cpp)
file(GLOB SRCSAXHANDLER_INCLUDE ../../srcSAX/src/srcsax/*.h* ../../srcSAX/src/cpp/*.h*)
file(GLOB SRCSLICEHANDLER_SOURCE cpp/*.cpp)
file(GLOB SRCSLICERUNTIME_INCLUDE release/*.cpp)
file(GLOB SRCSLICEHANDLER_INCLUDE headers/*.hpp)

add_executable(srcSlice ${SRCSLICERUNTIME_INCLUDE} ${SRCSLICEHANDLER_SOURCE} ${SRCSLICEHANDLER_INCLUDE})

target_link_libraries(srcSlice srcsax ${LIBXML2_LIBRARIES})