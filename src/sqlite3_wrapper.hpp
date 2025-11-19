#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

#include <sqlite3.h>

namespace detail__ {
class DBCon {
public:
    DBCon(const std::filesystem::path& path);

    ~DBCon();

    DBCon(const DBCon&) = delete;
    DBCon& operator=(const DBCon&) = delete;
 
public:
    std::vector<
        std::vector<std::pair<std::string, std::string>>> 
    exec(const std::string& stm);

private:    
    sqlite3* con_;  
};

} // namespace detail__

namespace sqlite3_wrapper {
// TODO: тип столбца (с not null, primary key ...)
// TODO: addRow...
class DB {
public:
    DB(const std::filesystem::path& path);

public:
    void createTable(
        const std::string& name, 
        const std::pair<std::string, std::string>& primaryKey,  
        const std::vector<std::pair<std::string, std::string>>& entries);

    template <class ... T>
    std::vector<std::tuple<T...>> 
    getAllRows(const std::string& tableName) {
        auto rows = con_.exec("select * from " + tableName);
        std::vector<std::tuple<T...>> res;
        res.reserve(rows.size());
        for (auto&& row : res) {
            res.push_back(rows_<T...>(row, 0));
        }
    }

    // void addRow(
    //     const std::string& tableName, 
        
    // );

private:    
    template <class T>
    std::tuple<T> rows_(
        const std::vector<
            std::pair<
                std::string, std::string>>& rows, 
        int idx
    ) 
    {
        auto&& [_, val] = rows.at(idx);
        if constexpr (std::is_same_v<T, int>) {
            return std::tuple<int>(std::stoi(val));
        } else {
            return std::tuple<std::string>(val);
        }
    }

    template <class T, class ... U>
    requires (sizeof...(U) > 0)
    std::tuple<T, U...> rows_(
        const std::vector<
            std::pair<
                std::string, std::string>>& rows, 
        int idx
    ) 
    {   
        auto r1 = rows_<T>(rows, idx++);
        auto r2 = rows_<U...>(rows, idx);
        return std::tuple_cat(std::move(r1), 
                              std::move(r2));
    }
    
private:
    detail__::DBCon con_;
};

} // namespace sqlite3_wrapper