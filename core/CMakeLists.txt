cmake_minimum_required(VERSION 3.00)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES earley.cpp grammar.cpp parser.cpp reader.cpp)

add_library(Everything SHARED ${SOURCE_FILES})
