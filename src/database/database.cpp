#include "database.hpp"

namespace todod::db {

DataBase::DataBase(std::string& path) : 
    db_(
        path, 
        SQLite::OPEN_READWRITE | 
        SQLite::OPEN_CREATE
    ) 
{
    db_.exec("PRAGMA foreign_key = ON");
}

SQLite::Database& DataBase::connection() noexcept {
    return db_;
}

std::mutex& DataBase::mutex() noexcept {
    return mutex_;
}

} // namespace todod::db 