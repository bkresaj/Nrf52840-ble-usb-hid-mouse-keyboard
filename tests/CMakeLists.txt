cmake_minimum_required(VERSION 3.20)

project(unit_tests)

enable_testing()

set(PROJECT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")
set(OUTPUT_TEST_BUILD_DIRECTORY
    "${PROJECT_DIRECTORY}tests/build")

# build google test together with the project
execute_process(COMMAND ${CMAKE_COMMAND} .
                WORKING_DIRECTORY /root/gtest/googletest)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
                WORKING_DIRECTORY /root/gtest/googletest)
add_subdirectory(/root/gtest/googletest /root/gtest/googletest/build
                 EXCLUDE_FROM_ALL)

add_subdirectory(unit_tests)
