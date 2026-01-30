#include <iostream>
#include <exception>

#include <gtest/gtest.h>

#include "sqlite3_wrapper.hpp"

#ifndef TEST_DB_FILE_PATH
#error "It is necessary to declare"  
       " the file name of the test db"
#endif

TEST(DbCreation, DefaultCreation) {
    try {
        sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);
    } catch (const std::exception& e) {
        ASSERT_FALSE(true) << e.what();
    } catch (...) {
        ASSERT_FALSE(true) << "Unknown error";
    }
}

TEST(TableCreation, Table1) {
    sqlite3_wrapper::DB db(TEST_DB_FILE_PATH);
    try {
        db.createTable("table1", 
            {std::string("id"), std::string("INTEGER")}, 
            {{std::string("name"), std::string("TEXT")}});
    } catch (const std::exception& e) {
        ASSERT_FALSE(true) << e.what();
    } catch (...) {
        ASSERT_FALSE(true) << "Unknown error";
    }

}