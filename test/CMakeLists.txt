# Remove find_package(ryu REQUIRED)

# Set library manually
set(RYU_INCLUDE_DIR /Users/luccalabattaglia/Work/ryu/include)
set(RYU_LIB /Users/luccalabattaglia/Work/ryu/build/libryu.a)

# Test: Logging
add_executable(test_logging logging.cpp)
target_include_directories(test_logging PRIVATE ${RYU_INCLUDE_DIR})
target_link_libraries(test_logging PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME LoggingTest COMMAND test_logging)

# Test: Constants
add_executable(test_constants constants.cpp)
target_include_directories(test_constants PRIVATE ${RYU_INCLUDE_DIR})
target_link_libraries(test_constants PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME ConstantsTest COMMAND test_constants)

# Test: MagicMath
add_executable(test_magic_math magic_math.cpp)
target_include_directories(test_magic_math PRIVATE ${RYU_INCLUDE_DIR})
target_link_libraries(test_magic_math PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME MagicMathTest COMMAND test_magic_math)

# Test: Assembly
add_executable(test_asm rand_asm_test.cpp)
target_link_libraries(test_asm PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME TestAsm COMMAND test_asm)

# Numeric subdirectory
add_subdirectory(numeric)
