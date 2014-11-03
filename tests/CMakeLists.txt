enable_testing()

add_executable(TensorTest TensorTest.cpp)

add_executable(TriangleTest TriangleTest.cpp)
target_link_libraries(TriangleTest ${ZLIB_LIBRARY} ${OPENCL_LIBRARY})

add_executable(ConfigurationTest ConfigurationTest.cpp)
target_link_libraries(ConfigurationTest LINK_PUBLIC configparser)
configure_file(test.cfg ${CMAKE_CURRENT_BINARY_DIR}/test.cfg COPYONLY)
configure_file(../sim/MPMKernels.cl ${CMAKE_CURRENT_BINARY_DIR}/MPMKernels.cl COPYONLY)

add_executable(GraphTest GraphTest.cpp)

add_test(Tensor TensorTest)
add_test(Configuration ConfigurationTest)
add_test(Triangle TriangleTest)
add_test(Graph GraphTest)
