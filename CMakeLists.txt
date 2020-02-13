cmake_minimum_required(VERSION 3.5)
project(softRender LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(headers)
set(HEADER_LIST)
set(SOURCE_LIST)
add_executable(${PROJECT_NAME} main.cc ${HEADER_LIST} ${SOURCE_LIST})

