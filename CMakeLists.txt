cmake_minimum_required(VERSION 3.0)
project(dumbshell)
set(CMAKE_CXX_FLAGS "-g -Wall -O0 -std=c++11")

# Micro Parser Combinators (https://github.com/orangeduck/mpc)
# Checked out at f65b32b8421cf3954c7b73acbc4e77bace41cec4
add_library(mpc lib/mpc/mpc.c)

# Boost 1.55.0
find_package(Boost 1.55.0 REQUIRED)
include_directories(${Boost_include_dir})

# DumbShell as a library
add_library(libdsh src/mode.cc src/parse.cc src/logic.cc src/subprocess.cc)
target_link_libraries(libdsh mpc ${Boost_libraries})

# DumbShell entry point
add_executable(dsh src/main.cc)
target_link_libraries(dsh libdsh)

# Tests
#find_library(check_lib check NAMES check)
#add_executable(all_tests tests/check_logic.cc tests/check_parse.cc tests/checks.cc)
#target_link_libraries(all_tests dsh_lib ${check_lib})
#add_custom_target(check COMMAND all_tests DEPENDS all_tests)
