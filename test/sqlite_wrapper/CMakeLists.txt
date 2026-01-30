include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/54915d462bb64e9b2bd1a3f0e70c00e6f5d817fb.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(sqlite_wrapper_test main.cpp)

target_link_libraries(sqlite_wrapper_test PRIVATE sqlite3_wrapper)
target_link_libraries(sqlite_wrapper_test PRIVATE GTest::gtest_main)

set(TEST_DB_FILE_NAME "test")

set(TEST_DB_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TEST_DB_FILE_NAME}.db")

target_compile_definitions(
    sqlite_wrapper_test PRIVATE 
    TEST_DB_FILE_PATH="${TEST_DB_FILE_PATH}")   

add_custom_command(
    TARGET  sqlite_wrapper_test 
    POST_BUILD 
    COMMAND echo "" > ${TEST_DB_FILE_PATH})

add_test(NAME sqlite_wrapper_test COMMAND sqlite_wrapper_test)
