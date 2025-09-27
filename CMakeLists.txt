cmake_minimum_required(VERSION 3.15)

project(ouc_server VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -g -O2")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(test "${PROJECT_SOURCE_DIR}/example/epoll_tcp_loop.cpp")