# Helper function to reduce "CMake shit"
function(add_logging_test name src)
  add_executable(${name} ${src})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_logging_test(maybe maybe.cpp)
add_logging_test(maybe2 maybe2.cpp)
add_logging_test(maybe5k maybe5k.cpp)
add_logging_test(maybe5k2 maybe5k2.cpp)
