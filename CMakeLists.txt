cmake_minimum_required(VERSION 3.20)
project(OpenLogging CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Boost (header-only usage)
# ------------------------------------------------------------
find_package(Boost REQUIRED)

# ------------------------------------------------------------
# Build type
# ------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "" FORCE)
endif()

# ------------------------------------------------------------
# Compiler warnings (portable)
# ------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
      -g
      -O0
      -Wall
      -Wshadow
      -Werror=shadow
      -Wnull-dereference
      -fno-omit-frame-pointer)
  endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
      -Ofast
      -fprefetch-loop-arrays
      -funroll-loops
      -march=broadwell
      -static-libstdc++
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
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
      -Ofast
      -fprefetch-loop-arrays
      -funroll-loops
      -march=broadwell
      -static-libstdc++
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
endif()

# ------------------------------------------------------------
# Header-only library
# ------------------------------------------------------------
add_library(OpenLogging INTERFACE)

target_include_directories(
  OpenLogging
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../ryu # FIXED (relative path)
)

# Optional: propagate Boost headers
target_link_libraries(OpenLogging INTERFACE Boost::headers)

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------
enable_testing()
add_subdirectory(test)
