cmake_minimum_required(VERSION 3.0)
project(drss_mitm CXX C)

set(CMAKE_CXX_STANDARD 17)

configure_file(CMakeLists.txt.in rasta-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rasta-download)
if (result)
    message(FATAL_ERROR "CMake step for RaSTA failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rasta-download)
if (result)
    message(FATAL_ERROR "Build step for RaSTA failed: ${result}")
endif ()

# Add rasta directly to our build.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/rasta-src
        ${CMAKE_CURRENT_BINARY_DIR}/rasta-build
        EXCLUDE_FROM_ALL)


add_executable(signal_simulator attack-clients/simple-mitm/signal_simulator.cxx attack-clients/simple-mitm/hexdump.h)

add_library(rasta-wrapper rasta-wrapper/rasta_wrapper.cpp rasta-wrapper/rasta_wrapper.h)

target_link_libraries(rasta-wrapper PRIVATE rasta)

target_link_libraries(signal_simulator rasta-wrapper)

target_include_directories(rasta-wrapper PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/rasta-src/src/rasta/headers/")
target_link_directories(rasta-wrapper PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/rasta-build/bin/lib/")

# consumers of rasta-wrapper should set include path
target_include_directories(rasta-wrapper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rasta-wrapper)