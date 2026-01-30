cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(todoD)

enable_testing()

add_subdirectory("${CMAKE_SOURCE_DIR}/src/sqlite_wrapper")
add_subdirectory("${CMAKE_SOURCE_DIR}/test/sqlite_wrapper")

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3_wrapper)
