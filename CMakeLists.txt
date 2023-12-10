cmake_minimum_required(VERSION 3.0)

project(LR1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(LR1 main.cpp)
add_compile_definitions(GRAMMAR_PATH=\"${CMAKE_SOURCE_DIR}/grammar.txt\")

add_subdirectory(core)
target_include_directories(Everything PUBLIC include/)

target_link_libraries(LR1 Everything)
