##
# @brief Contains functions for setting an application that uses  Gap libraries
##
if (CMAKE_MINIMUM_REQUIRED_VERSION  VERSION_LESS 3.0.0)
    message(AUTHOR_WARNING "Your project should require atleast CMake 3.0 FindGap to work")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

##
# @brief Intialize a target with the given name
# @param name The name of the target
##
function(Gap_InitializeTarget name)
    if (NOT ${name}_TARGET_INITIALIZED)
        # Find all the source files
        file(GLOB ${name}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
        # Additional source files
        set(${name}_SOURCES ${${name}_SOURCES} ${${name}_ADDITIONAL_SOURCES} PARENT_SCOPE)
        # We need a configuration file
        set(${name}_CONFIG  ${name}.json PARENT_SCOPE)

        # Supply developer with debug macro
        if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
            add_definitions(-DNDEBUG)
        endif()

        set(${name}_TARGET_INITIALIZED 1 PARENT_SCOPE)
        include_directories(src)
    endif()
    message(STATUS "Target configured successfully, ${${name}_SOURCES}")
endfunction()

##
# @brief Loads the dependencies required by gap and libgap itself
# @param name The name of the project
##
function(Gap_LoadDependecies name)
    if (NOT ${name}_GAP_PROJECT_SETUP)
        if (NOT Boost_FOUND)
            # Dependency on boost
            find_package(Boost 1.60 REQUIRED COMPONENTS system filesystem date_time thread regex)
            if (NOT Boost_FOUND)
                message(FATAL_ERROR "This project has a hard dependency on boost libraries >= 1.60")
            endif()
        endif()
        set(${name}_LIBRARIES ${${name}_LIBRARIES} ${Boost_LIBRARIES})

        if (NOT Threads_FOUND)
            # Dependency on threading libraries
            find_package(Threads REQUIRED)
            if (NOT Threads_FOUND)
                message(FATAL_ERROR "Threading (pthreads) library need to be installed")
            endif()
        endif()
        set(${name}_LIBRARIES ${${name}_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

        if (${name}_OPENSSL_ENABLED)
            # If OpenSSL is enabled, find it
            if (NOT OpenSSL_FOUND)
                find_package(OpenSSL REQUIRED)
                if (NOT OpenSSL_FOUND)
                    message(FATAL_ERROR "This project has a hard dependency on OpenSSL")
                endif()
            endif()
            add_definitions(-DOPENSSL_ENABLED)
        endif()
        set(${name}_LIBRARIES ${${name}_LIBRARIES} ${OPENSSL_LIBRARIES})

        # Find gab_library and include
        if (NOT Gap_FOUND)
            if (NOT GAP_DEV_BINARY_DIR)
                set(GAP_DEV_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
            endif()

            # Find the include directory for gap header
            FIND_PATH( Gap_INCLUDE_PATH gap/gap.h
                       PATHS    ${GAP_DEV_BINARY_DIR}/include)

            # Find the library directory for gap header
            FIND_LIBRARY( Gap_LIBRARY
                    NAMES libgap.a
                    PATHS ${GAP_DEV_BINARY_DIR}/lib)

            # Include add the folder to list of include directories
            if (NOT Gap_INCLUDE_PATH)
                message(FATAL_ERROR "Gap library not found, ensure that Gap was installed ${GAP_DEV_BINARY_DIR}")
            endif()

            # Add library to list of libraries
            if (Gap_LIBRARY)
                set(Gap_FOUND 1 CACHE STRING "Set to 1 if library found, 0 otherwise")
            else()
                message(FATAL_ERROR "Gap library not found, ensure that Gap is installed in build system")
            endif()
            message(STATUS "Include - ${Gap_INCLUDE_PATH} Lib ${Gap_LIBRARY}")
            set(Gap_Found 1)
        endif()
        include_directories(${Gap_INCLUDE_PATH})
        set(${name}_LIBRARIES ${${name}_LIBRARIES} ${Gap_LIBRARY})

        # Append additional libraries
        if (${name}_ADDITIONAL_LIBRARIES)
            set(${name}_LIBRARIES ${${name}_LIBRARIES} ${${name}_ADDITIONAL_LIBRARIES})
        endif()
        set(${name}_LIBRARIES ${${name}_LIBRARIES} PARENT_SCOPE)
        message(STATUS "Target ${name} successfully configured, libraries ${${name}_LIBRARIES}")
        set(${name}_GAP_PROJECT_SETUP 1 PARENT_SCOPE)
    else()
        message(STATUS "Target ${name} already called configured, libraries ${${name}_LIBRARIES}")
    endif()
endfunction()

##
# Adds a target that can be deployed to suil, for local debuging through gatls run, used AddDebugExec
# @param name: the name of the target
##
function(Gap_AddTarget name)
    if (NOT ${name}_TARGET_ADDED)
        add_library(${name} SHARED ${${name}_SOURCES})
        if (${name}_ADDITIONAL_DEFINES)
            target_compile_definitions(${name} PUBLIC "${COMPILE_DEFINATIONS} ${${name}_ADDITIONAL_DEFINES}")
        endif()

        target_link_libraries(${name} ${${name}_LIBRARIES})
        set_target_properties(${name} PROPERTIES PREFIX "" SUFFIX ".gap")

        set(${name}_TARGET_ADDED 1 PARENT_SCOPE)
    endif()
endfunction()

##
# Finalizes the target by moving the runtime files to bin directory
# @param name: the name of the target
##
function(Gap_FinalizeTarget name)
    if (NOT ${name}_TARGET_FINALIZED)
        if (NOT GAP_BINARY_PREFIX)
            set(GAP_BINARY_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin)
        endif()

        # We need to install all the required files
        INSTALL(TARGETS ${name} DESTINATION ${GAP_BINARY_PREFIX})
        INSTALL(FILES   ${${name}_CONFIG} DESTINATION ${GAP_BINARY_PREFIX})
        set(${name}_TARGET_FINALIZED 1 PARENT_SCOPE)
    endif()
endfunction()