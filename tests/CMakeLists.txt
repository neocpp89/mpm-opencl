enable_testing()

add_executable(TensorTest TensorTest.cpp)

add_executable(TriangleTest TriangleTest.cpp)
target_link_libraries(TriangleTest ${ZLIB_LIBRARY} ${OPENCL_LIBRARY})
configure_file(../sim/MPMKernels.cl ${CMAKE_CURRENT_BINARY_DIR}/MPMKernels.cl COPYONLY)

add_executable(ConfigurationTest ConfigurationTest.cpp)
target_link_libraries(ConfigurationTest LINK_PUBLIC configparser)
configure_file(test.cfg ${CMAKE_CURRENT_BINARY_DIR}/test.cfg COPYONLY)

add_executable(GraphTest GraphTest.cpp)

add_executable(MshTest MshTest.cpp)
target_link_libraries(MshTest LINK_PUBLIC mshutils)
configure_file(../py/example.msh ${CMAKE_CURRENT_BINARY_DIR}/example.msh  COPYONLY)

add_executable(SpatialBinningTest SpatialBinningTest.cpp)
target_link_libraries(SpatialBinningTest ${ZLIB_LIBRARY} ${OPENCL_LIBRARY})
configure_file(../sim/MPMKernels.cl ${CMAKE_CURRENT_BINARY_DIR}/MPMKernels.cl COPYONLY)

add_test(Tensor TensorTest)
add_test(Configuration ConfigurationTest)
add_test(Triangle TriangleTest)
add_test(Graph GraphTest)
add_test(Msh MshTest)
add_test(SpatialBinning SpatialBinningTest)

# Add tests in a semi-automated manner.

file(GLOB UnitTests ./ "*Test.cpp")
list(APPEND UnitTests "catchmain.cpp")
add_executable(unit ${UnitTests})
target_link_libraries(unit mshutils)
target_link_libraries(unit configparser)
target_link_libraries(unit ${ZLIB_LIBRARY})
target_link_libraries(unit ${OPENCL_LIBRARY})
add_test(unit unit)
