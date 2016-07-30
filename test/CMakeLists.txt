CMAKE_MINIMUM_REQUIRED( VERSION 3.0)

file(GLOB GAREST_SOURCES "*.cpp")
set(GAREST_SOURCES
        ${SUIL_COMMON_SOURCES}
        ${SUIL_GAP_SOURCES}
        ${GAR_SOURCES}
        ${GAREST_SOURCES})

add_executable(garest ${GAREST_SOURCES})
target_link_libraries(garest
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENSSL_LIBRARIES}
        dl
        gtest gtest_main)