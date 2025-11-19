cmake_minimum_required(VERSION 3.20)
project(OpenLogging CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json in build/
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only library
add_library(OpenLogging INTERFACE)
target_include_directories(OpenLogging INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Add executable for test.cpp
add_executable(Test test.cpp)
target_link_libraries(Test PRIVATE OpenLogging)

