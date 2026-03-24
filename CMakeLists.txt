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
target_include_directories(OpenLogging INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                                                 /home/lucca/Work/ryu)

# Test executable
add_executable(test_logging ${CMAKE_CURRENT_SOURCE_DIR}/test/logging.cpp)
add_executable(test_constants ${CMAKE_CURRENT_SOURCE_DIR}/test/constants.cpp)
add_executable(test_numeric_integers
               ${CMAKE_CURRENT_SOURCE_DIR}/test/numeric/integers.cpp)
add_executable(
  test_numeric_floating_exponential_notation
  ${CMAKE_CURRENT_SOURCE_DIR}/test/numeric/floating/exponential_notation.cpp)

add_executable(
  test_numeric_floating_n_digits_precision
  ${CMAKE_CURRENT_SOURCE_DIR}/test/numeric/floating/n_digits_precision.cpp)

target_link_libraries(test_logging PRIVATE OpenLogging
                                           /home/lucca/Work/ryu/build/libryu.a)
target_link_libraries(test_constants
                      PRIVATE OpenLogging /home/lucca/Work/ryu/build/libryu.a)

target_link_libraries(test_numeric_integers
                      PRIVATE OpenLogging /home/lucca/Work/ryu/build/libryu.a)

target_link_libraries(test_numeric_floating_exponential_notation
                      PRIVATE OpenLogging /home/lucca/Work/ryu/build/libryu.a)

target_link_libraries(test_numeric_floating_n_digits_precision
                      PRIVATE OpenLogging /home/lucca/Work/ryu/build/libryu.a)
