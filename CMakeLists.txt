cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include(cmake/address_sanitizer.cmake) 
include(cmake/thread_sanitizer.cmake)

option(ENABLE_THREAD_SANITIZER OFF)
option(ENABLE_ADDRESS_SANITIZER OFF)
option(ENABLE_LOG OFF)

project(todod)

find_package(nlohmann_json 3 REQUIRED)

enable_testing()

if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/src/sqlite_wrapper")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/http_1_0_server")

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
target_link_libraries(${PROJECT_NAME} PRIVATE http_1_0_server)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3_wrapper)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

if (ENABLE_LOG) 
    target_compile_definitions(${PROJECT_NAME} PRIVATE LOG)
endif()

if (ENABLE_THREAD_SANITIZER)
    add_thread_sanitizer(${PROJECT_NAME}) 
endif()

if (ENABLE_ADDRESS_SANITIZER)
    add_address_sanitizer(${PROJECT_NAME})
endif()
