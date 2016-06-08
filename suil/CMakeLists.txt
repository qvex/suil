CMAKE_MINIMUM_REQUIRED( VERSION 3.0)

file(GLOB GAREST_SOURCES "*.cpp")
set(SUIL_SOURCES
        ${GAR_COMMON_SOURCES}
        ${GAR_GAR_SOURCES}
        ${GAR_GAP_SOURCES}
        ${GAREST_SOURCES})

add_executable(suil ${SUIL_SOURCES})
target_link_libraries(suil
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENSSL_LIBRARIES})