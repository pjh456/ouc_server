cmake_minimum_required(VERSION 3.15)

project(ouc_server VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -g -O2")

add_subdirectory(${PROJECT_SOURCE_DIR}/include/ouc_server)

add_executable(test "${PROJECT_SOURCE_DIR}/examples/test_thread_pool.cpp")
target_link_libraries(test PRIVATE ouc_server_lib)