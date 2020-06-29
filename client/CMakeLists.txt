cmake_minimum_required(VERSION 3.17.0)
project(interview_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(client main.cpp client_tcp.cpp client_udp.cpp)