# Helper function to reduce "CMake shit"
function(add_logging_test name src)
  add_executable(${name} ${src})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_logging_test(maybe maybe.cpp)
add_logging_test(maybe2n maybe2n.cpp)
add_logging_test(maybe2n_base maybe2n_base.cpp)
add_logging_test(maybe5k maybe5k.cpp)
add_logging_test(maybe5k_base maybe5k_base.cpp)
