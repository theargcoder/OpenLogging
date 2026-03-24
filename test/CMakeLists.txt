set(RYU_LIB /home/lucca/Work/ryu/build/libryu.a)

# Test: Logging
add_executable(test_logging logging.cpp)
target_link_libraries(test_logging PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME LoggingTest COMMAND test_logging)

# Test: Constants
add_executable(test_constants constants.cpp)
target_link_libraries(test_constants PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME ConstantsTest COMMAND test_constants)

add_subdirectory(numeric)
