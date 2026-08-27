cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(todod)

include(cmake/address_sanitizer.cmake) 
include(cmake/thread_sanitizer.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLiteCpp REQUIRED)
find_package(Lua 5.4 REQUIRED)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC} ${INCLUDE})

target_link_libraries(todod PRIVATE SQLiteCpp)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/crow"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sol"
        "${LUA_INCLUDE_DIR}"
    
)    
    
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${LUA_LIBRARIES}
)

set(TODOD_DOCS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    TODOD_DOCS_DIR="${TODOD_DOCS_OUTPUT_DIR}"
)

add_custom_target(todod_docs ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/docs"
        "${TODOD_DOCS_OUTPUT_DIR}"
    COMMENT "Copying API documentation"
)

add_dependencies(${PROJECT_NAME} todod_docs)
