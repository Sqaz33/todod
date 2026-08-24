cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(todod)

include(cmake/address_sanitizer.cmake) 
include(cmake/thread_sanitizer.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLiteCpp REQUIRED)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC} ${INCLUDE})

target_link_libraries(todod PRIVATE SQLiteCpp)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/crow"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sol"
    )