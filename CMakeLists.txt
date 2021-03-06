cmake_minimum_required(VERSION 3.0)
project(suil CXX C)

set(SUIL_MAJOR_VERSION  0)
set(SUIL_MINOR_VERSION  0)
set(SUIL_PATCH_VERSION  0)
set(SUIL_VERSION
        ${SUIL_PATCH_VERSION}.${SUIL_PATCH_VERSION}.${SUIL_PATCH_VERSION})

# generate suil header file
configure_file(
        "${PROJECT_SOURCE_DIR}/suil.h.in"
        "${PROJECT_BINARY_DIR}/suil.h"
)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}:${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Determine if we should build tests
option(SUIL_BUILD_TESTS     "Determines if building tests is enabled or not" ON)
option(SUIL_LOGSOURCE       "Determines whether the source of the log should be included in logs" ON)
option(SUIL_OPENSSL_ENABLED "Determines if OpenSSL is enabled with this build" ON)
option(GAR_STATS_ENABLED    "Enables tracking of statistics" ON)
option(SUIL_BUILD_EXAMPLES  "Enables build of examples" ON)

if (NOT SUIL_INSTALL_PREFIX)
    set(SUIL_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)
endif()

# Defaukt the build type to debug builds
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Configure Debug build definitions
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    if (SUIL_LOGSOURCE)
        add_definitions(-DSUIL_ENABLE_LOGSRC)
    endif()
    if (GAR_STATS_ENABLED)
        add_definitions(-DGAR_STATS_ENABLED)
    endif ()
    add_definitions(-DSUIL_DEBUG)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Get a list of all common source files
file(GLOB SUIL_COMMON_SOURCES    "common/*.cpp")
file(GLOB GAR_SOURCES           "gar/*.cpp" "gar/*.c")
file(GLOB SUIL_GAP_SOURCES       "gap/*.cpp" "gap/*.c")

# Dependency on boost
find_package(Boost 1.60 REQUIRED COMPONENTS system filesystem date_time thread regex)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "This project has a hard dependency on boost libraries >= 1.60")
endif()
# Dependency on threading libraries
find_package(Threads REQUIRED)
if (NOT Threads_FOUND)
    message(FATAL_ERROR "Threading (pthreads) library need to be installed")
endif()

# If OpenSSL is enabled, find it
if (GAR_OPENSSL_ENABLED)
    find_package(OpenSSL REQUIRED)
    if (NOT OpenSSL_FOUND)
        message(FATAL_ERROR "This project has a hard dependency on OpenSSL")
    else()
        add_definitions(-DSUIL_OPENSSL_ENABLED)
    endif()
endif()

include_directories(common gar gap ${PROJECT_BINARY_DIR})

add_subdirectory(gap)
add_subdirectory(gatls)

if (SUIL_BUILD_TESTS)
    add_subdirectory(lib/googletest)
    include_directories(${CMAKE_SOURCE_DIR}/lib/googletest/googletest/include)
    add_definitions(-DSUIL_UNIT_TEST_BUILD)
    add_subdirectory(test)
endif()

if (SUIL_BUILD_EXAMPLES)
    #add_subdirectory(examples/echo)
endif()