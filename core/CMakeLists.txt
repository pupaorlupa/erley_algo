cmake_minimum_required(VERSION 3.00)

set(SOURCE_FILES LR1.cpp grammar.cpp parser.cpp reader.cpp)

add_library(Everything SHARED ${SOURCE_FILES})
