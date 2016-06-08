CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
project(gap)

# all source files in this folder are used to build GAP
file(GLOB GAP_LOCAL_SOURCES "*.cpp")
set(GAP_SOURCES ${SUIL_COMMON_SOURCES} ${GAP_LOCAL_SOURCES})

# GAP include files to be installed together with the library
set(GAP_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/common/json.h
        ${CMAKE_SOURCE_DIR}/common/log.h
        ${CMAKE_SOURCE_DIR}/common/query_string.h
        ${CMAKE_SOURCE_DIR}/common/request.h
        ${CMAKE_SOURCE_DIR}/common/response.h
        ${CMAKE_SOURCE_DIR}/common/server_options.h
        ${CMAKE_SOURCE_DIR}/common/util.h
        app.h
        gap.h
        middleware.h
        middleware_factory.h
        middleware_pipeline.h
        param_value.h
        resource_handler.h
        resource_router.h
        route.h
        route_params.h
        route_params_parser.h
        trie.h)

# Build gap as a static library
add_library(gap STATIC ${GAP_SOURCES})
target_link_libraries(gap
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENSSL_LIBRARIES})

INSTALL(TARGETS gap DESTINATION ${SUIL_INSTALL_PREFIX}/lib)
INSTALL(FILES   ${GAP_INCLUDE_FILES} DESTINATION ${SUIL_INSTALL_PREFIX}/include/gap)