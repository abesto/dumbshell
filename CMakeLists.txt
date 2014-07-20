cmake_minimum_required(VERSION 3.0)
project(dumbshell)
set(CMAKE_CXX_FLAGS "-g -Wall -O0 -std=c++11")

# Boost 1.55.0
find_package(Boost 1.55.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_include_dir})

# DumbShell as a library
add_library(libdsh src/mode.cc src/parse.cc src/logic.cc src/subprocess.cc)

# DumbShell entry point
add_executable(dsh src/main.cc)
target_link_libraries(dsh libdsh)

# Tests
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN)
add_executable(dsh_tests tests/main.cc tests/check_logic.cc tests/check_parse.cc)
target_link_libraries(dsh_tests libdsh ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
## link with a main function
add_custom_target(check COMMAND dsh_tests DEPENDS dsh_tests)
