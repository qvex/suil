cmake_minimum_required(VERSION 3.5)
project(gar CXX)

# Determine if we should build tests
option(GAR_BUILD_TESTS "Determines if building tests is enabled or not" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Get a list of all common source files
file(GLOB GAR_COMMON_SOURCES "common/*.cpp")

if (GAR_BUILD_TESTS)
    add_subdirectory(test)
endif()