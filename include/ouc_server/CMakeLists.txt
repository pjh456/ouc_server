cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)
file(GLOB_RECURSE SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)

add_library(ouc_server_lib ${SOURCE_FILE} ${HEADER_FILE})

target_include_directories(ouc_server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})