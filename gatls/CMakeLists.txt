CMAKE_MINIMUM_REQUIRED( VERSION 3.0)
project(gatls)

file(GLOB GATLS_LOCAL_SOURCES "*.cpp")
set(GATLS_SOURCES
        ${SUIL_COMMON_SOURCES}
        ${GAR_SOURCES}
        ${GATLS_LOCAL_SOURCES})

add_executable(gatls ${GATLS_SOURCES})
target_link_libraries(gatls
        ${Boost_LIBRARIES}
        gap
        dl)

INSTALL(TARGETS gatls DESTINATION ${SUIL_INSTALL_PREFIX}/gatls)