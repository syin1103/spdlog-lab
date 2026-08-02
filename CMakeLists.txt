cmake_minimum_required(VERSION 3.16)

project(spdlog_lab VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt REQUIRED)
find_package(GTest REQUIRED)

add_library(spdlog_lab INTERFACE)
target_include_directories(spdlog_lab INTERFACE ${PROJECT_SOURCE_DIR})
target_link_libraries(spdlog_lab INTERFACE fmt::fmt)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*_test.cc")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE spdlog_lab GTest::gtest_main)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()