enable_testing()

add_executable(TensorTest TensorTest.cpp)

add_executable(ConfigurationTest ConfigurationTest.cpp)
target_link_libraries(ConfigurationTest LINK_PUBLIC configparser)
configure_file(test.cfg ${CMAKE_CURRENT_BINARY_DIR}/test.cfg COPYONLY)
configure_file(../sim/rotation.cl ${CMAKE_CURRENT_BINARY_DIR}/test.cl COPYONLY)

add_executable(t1 test.cpp)

add_test(inital t1)
add_test(Tensor TensorTest)
add_test(Configuration ConfigurationTest)
