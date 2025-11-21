cmake_minimum_required(VERSION 3.20)
project(OpenLogging CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Debug config
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")

# Header-only library
add_library(OpenLogging INTERFACE)
target_include_directories(OpenLogging INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Test executable
add_executable(Test test.cpp)
target_link_libraries(Test PRIVATE OpenLogging)

