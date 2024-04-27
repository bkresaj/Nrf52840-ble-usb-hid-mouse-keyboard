cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME ut_example)

add_executable(${TARGET_NAME} ut_example.cpp)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC gtest gtest_main)