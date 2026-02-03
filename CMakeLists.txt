cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(todoD)

find_package(nlohmann_json 3 REQUIRED)

enable_testing()

add_subdirectory("${CMAKE_SOURCE_DIR}/src/sqlite_wrapper")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/http_1_0_server")

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE http_1_0_server)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3_wrapper)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)


