file(GLOB WRAPPER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(SQLite3 REQUIRED)

add_library(sqlite3_wrapper SHARED ${WRAPPER_SRC})

target_link_libraries(sqlite3_wrapper PRIVATE SQLite::SQLite3)

target_include_directories(sqlite3_wrapper INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")