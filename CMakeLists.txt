cmake_minimum_required(VERSION 3.10)

project(chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/common/include)

add_subdirectory(client)
add_subdirectory(server)
