cmake_minimum_required(VERSION 3.17.0)
project(interview_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(server main.cpp server_tcp.cpp server_udp.cpp parser.cpp)

target_link_libraries(server pthread)