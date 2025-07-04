cmake_minimum_required(VERSION 3.25)
project(iec104_server_simulator)

set(ABSL_PROPAGATE_CXX_STD ON)
set(CMAKE_CXX_STANDARD 20)
set(LIB_60870 vendor/lib60870-C)
set(LIB_SPDLOG vendor/spdlog)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

add_executable(
        iec104_server_simulator
        main.cpp
        include/nlohmann/json.hpp
        include/flags/flags.h
        include/helpers/strings.h
        include/helpers/logger.h
        src/callbacks.cpp
        src/callbacks.h
        src/config.cpp
        src/config.h
        src/dummy_readings.cpp
        src/dummy_readings.h
        src/iec104_server_simulator.h
        src/timestamps.cpp
        src/timestamps.h
        src/utils.cpp
        src/utils.h
)

include_directories(
        include
        ${LIB_60870}/src/inc/api
        ${LIB_60870}/src/hal/inc
        ${LIB_SPDLOG}/include

)

add_subdirectory(${LIB_60870})
add_subdirectory(${LIB_SPDLOG})

link_directories(${CMAKE_BINARY_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(COCOA_LIBRARY Cocoa ONLY)
    find_library(SECURITY_LIBRARY Security ONLY)
    find_library(RESOLV_LIBRARY Resolv ONLY)
    target_link_libraries(
            iec104_server_simulator
            PUBLIC Threads::Threads
            lib60870
            spdlog
            ${COCOA_LIBRARY}
            ${SECURITY_LIBRARY}
            ${RESOLV_LIBRARY}
    )
else ()
    target_link_libraries(
            iec104_server_simulator
            PUBLIC Threads::Threads
            lib60870
            spdlog
    )
endif ()
