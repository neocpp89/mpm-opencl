enable_testing()

add_executable(TriangleTest TriangleTestCL.cpp)
target_link_libraries(TriangleTest ${ZLIB_LIBRARY} ${OPENCL_LIBRARY})
configure_file(../sim/MPMKernels.cl ${CMAKE_CURRENT_BINARY_DIR}/MPMKernels.cl COPYONLY)

add_executable(SpatialBinningTest SpatialBinningTestCL.cpp)
target_link_libraries(SpatialBinningTest ${ZLIB_LIBRARY} ${OPENCL_LIBRARY})
configure_file(../sim/MPMKernels.cl ${CMAKE_CURRENT_BINARY_DIR}/MPMKernels.cl COPYONLY)

add_test(Triangle TriangleTest)
add_test(SpatialBinning SpatialBinningTest)

# Add tests in a semi-automated manner.

file(GLOB UnitTests ./ "*Test.cpp")
list(APPEND UnitTests "catchmain.cpp")
add_executable(unit ${UnitTests})
target_link_libraries(unit mshutils)
target_link_libraries(unit configparser)
target_link_libraries(unit ${ZLIB_LIBRARY})
add_test(unit unit)
