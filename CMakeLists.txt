CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
PROJECT(mpm-opencl)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
SET(CMAKE_CXX_FLAGS "-march=native -std=c++11 -g -O3 -Wall -Wextra")

# generate string containing git hash.
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
enable_testing()

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure
        --build-config "$<CONFIGURATION>")
else ()
    add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure)
endif ()

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/sim
    ${PROJECT_SOURCE_DIR}/configparser
    ${PROJECT_SOURCE_DIR}/optionparser
    ${PROJECT_SOURCE_DIR}/mshutils
)
ADD_SUBDIRECTORY(sim)
ADD_SUBDIRECTORY(py)
ADD_SUBDIRECTORY(configparser)
ADD_SUBDIRECTORY(optionparser)
ADD_SUBDIRECTORY(mshutils)
ADD_SUBDIRECTORY(tests)

