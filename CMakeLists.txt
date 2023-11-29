cmake_minimum_required(VERSION 3.0)

project(EARLEY)
add_executable(EARLEY main.cpp)

add_subdirectory(core)
target_include_directories(Everything PUBLIC include/)

target_link_libraries(EARLEY Everything)
