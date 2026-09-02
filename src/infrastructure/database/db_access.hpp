#pragma once

class DataBase;

namespace todod::db {

class DBAccess {
private:
    explicit DBAccess() = default;
    friend class DataBase;
};

} // namespace todod::db