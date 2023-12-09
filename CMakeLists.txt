
project(LR1)
add_executable(LR1 LR1.cpp)

set(SOURCE_FILES grammar.cpp parser.cpp reader.cpp)
set (CMAKE_BUILD_TYPE Debug)
add_library(Everything SHARED ${SOURCE_FILES})
target_include_directories(Everything PUBLIC .)
target_include_directories(LR1 PUBLIC .)

add_dependencies(LR1 Everything)
target_link_libraries(LR1 Everything)
