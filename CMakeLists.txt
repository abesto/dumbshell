cmake_minimum_required(VERSION 2.8)
project(dumbshell)
set(CMAKE_C_FLAGS "-g -Wall -O0")

add_library(dsh_lib src/mode.c src/logic.c)

add_executable(dsh src/main.c)
target_link_libraries(dsh dsh_lib)

include_directories ("${PROJECT_SOURCE_DIR}/src")
find_library(check_lib check NAMES check)
add_executable(all_tests tests/check_logic.c tests/checks.c)
target_link_libraries(all_tests dsh_lib ${check_lib})

add_custom_target(check COMMAND all_tests DEPENDS all_tests)
