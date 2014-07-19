cmake_minimum_required(VERSION 3.0)
project(dumbshell)
set(CMAKE_C_FLAGS "-g -Wall -O0 -std=c99")

# Micro Parser Combinators (https://github.com/orangeduck/mpc)
# Checked out at f65b32b8421cf3954c7b73acbc4e77bace41cec4
add_library(mpc lib/mpc/mpc.c)
include_directories("${PROJECT_SOURCE_DIR}/lib/mpc")

# DumbShell as a library
add_library(dsh_lib src/mode.c src/logic.c src/subprocess.c)
target_link_libraries(dsh_lib mpc)

# DumbShell entry point
add_executable(dsh src/main.c)
target_link_libraries(dsh dsh_lib)

# Tests
include_directories ("${PROJECT_SOURCE_DIR}/src")
find_library(check_lib check NAMES check)
add_executable(all_tests tests/check_logic.c tests/checks.c)
target_link_libraries(all_tests dsh_lib ${check_lib})
add_custom_target(check COMMAND all_tests DEPENDS all_tests)
