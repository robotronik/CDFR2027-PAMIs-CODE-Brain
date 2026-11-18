cmake_minimum_required(VERSION 3.16)
project(brain_tests CXX)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

# protocol.cpp has no IDF includes, so it builds on the host
add_executable(test_protocol test_protocol.cpp ../components/control_link/src/protocol.cpp)
target_include_directories(test_protocol PRIVATE ../components/control_link/include)
add_test(NAME protocol COMMAND test_protocol)
