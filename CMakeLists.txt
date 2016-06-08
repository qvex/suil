cmake_minimum_required(VERSION 3.5)
project(gar CXX C)

# Determine if we should build tests
option(GAR_BUILD_TESTS "Determines if building tests is enabled or not" ON)
option(GAR_LOGSOURCE "Determines whether the source of the log should be included in logs" ON)
option(GAR_OPENSSL_ENABLED "Determines if OpenSSL is enabled with this build" ON)
option(GAR_STATS_ENABLED "Enables tracking of statistics" ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    if (GAR_LOGSOURCE)
        add_definitions(-DGAR_ENABLE_LOGSRC)
    endif()
    if (GAR_STATS_ENABLED)
        add_definitions(-DGAR_STATS_ENABLED)
    endif ()
    add_definitions(-DGAR_DEBUG)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Get a list of all common source files
file(GLOB GAR_COMMON_SOURCES    "common/*.cpp")
file(GLOB GAR_GAR_SOURCESC      "gar/*.c")
file(GLOB GAR_GAR_SOURCESCXX    "gar/*cpp")
set(GAR_GAR_SOURCES             ${GAR_GAR_SOURCESCXX} ${GAR_GAR_SOURCESC})
file(GLOB GAR_GAP_SOURCES       "gap/*cpp")

# Dependency on boost
find_package(Boost 1.60 REQUIRED COMPONENTS system date_time thread regex)
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
        add_definitions(-DGAR_OPENSSL_ENABLED)
    endif()
endif()

include_directories(common gar gap)

if (GAR_BUILD_TESTS)
    add_subdirectory(lib/googletest)
    add_subdirectory(test)
endif()
add_subdirectory(suil)