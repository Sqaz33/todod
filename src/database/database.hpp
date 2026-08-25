#pragma once

#include <mutex>

#include <SQLiteCpp/SQLiteCpp.h>

namespace todod::db {

class DataBase { 
public:
    explicit DataBase(const std::string& path);

public:
    SQLite::Database& connection() noexcept;
    std::mutex& mutex() noexcept;

private:
    std::mutex mutex_;
    SQLite::Database db_;
};

} // namespace todod::db
