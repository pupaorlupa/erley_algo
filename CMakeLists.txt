cmake_minimum_required(VERSION 3.0)

project(EARLEY)
add_executable(EARLEY test.cpp)

add_subdirectory(core)

target_link_libraries(EARLEY Everything)
