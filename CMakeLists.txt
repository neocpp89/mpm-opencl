CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
PROJECT(mpm-opencl)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
SET(CMAKE_CXX_FLAGS "-march=native -std=c++11 -g -O3 -Wall -Wextra")

FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(png++ REQUIRED)
INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIR})
enable_testing()

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure
        --build-config "$<CONFIGURATION>")
else ()
    add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure)
endif ()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/sim ${PROJECT_SOURCE_DIR}/configparser)
ADD_SUBDIRECTORY(sim)
ADD_SUBDIRECTORY(py)
ADD_SUBDIRECTORY(configparser)
ADD_SUBDIRECTORY(tests)

