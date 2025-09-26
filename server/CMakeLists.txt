cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME chat_server)

set(SRC_FILES
	src/main.cpp
)

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
