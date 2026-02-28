cmake_minimum_required(VERSION 3.20)
project(OpenLogging CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Debug config
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "" FORCE)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(
    -g
    -O0
    -Wall
    -Wshadow
    -Wshadow=compatible-local
    -Wshadow=local
    -Werror=shadow
    -Wnull-dereference
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -fno-omit-frame-pointer)
endif()

# Header-only library
add_library(OpenLogging INTERFACE)
target_include_directories(OpenLogging INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Test executable
add_executable(test test.cpp)
target_link_libraries(test PRIVATE OpenLogging)
