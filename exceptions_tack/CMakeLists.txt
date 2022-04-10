cmake_minimum_required(VERSION 3.21)
project(exceptions_tack)

add_library(exceptions exceptions.h)
set_property(TARGET exceptions PROPERTY CXX_STANDARD 17)
set_target_properties(exceptions PROPERTIES LINKER_LANGUAGE CXX)

add_executable(exceptions_test test_macros.cpp)
add_executable(exceptions_rethrowning test_container_throw.cpp)

target_link_libraries(exceptions_test exceptions)
target_link_libraries(exceptions_rethrowning exceptions)