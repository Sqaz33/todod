#include "sqlite3_wrapper.hpp"

#include <stdexcept>
#include <sstream>

namespace {

std::vector<
    std::pair<std::string, std::string>> rows;

int callback(
    void*, int argc, char** argv, char** azColName) 
{   
    rows.clear();
    rows.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        rows.push_back({azColName[i], 
                argv[i] ? argv[i] : "NULL"});
    }
    return 0;
}

} // namespace


namespace detail__ {

DBCon::DBCon(const std::filesystem::path& path) {
    auto notOk = sqlite3_open(path.c_str(), &con_);
    if (notOk) {
        throw std::runtime_error(
            "Cant't open DB: " + path.string());
    }
}

DBCon::~DBCon() {
    sqlite3_close(con_);
}

std::vector<
    std::vector<std::pair<std::string, std::string>>> 
DBCon::exec(const std::string& stm) {    
    std::vector<
        std::vector<
            std::pair<std::string, std::string>>> res;
    char* errMsg = nullptr;
    int ok = sqlite3_exec(
            con_, stm.c_str(), callback, nullptr, &errMsg);
    if (ok != SQLITE_OK) {
        std::runtime_error re(errMsg);
        sqlite3_free(errMsg);
        throw re;
    }
    return res;
}

} // namespace detail__

namespace sqlite3_wrapper {
DB::DB(const std::filesystem::path& path) :
    con_(detail__::DBCon(path))
{}

void DB::createTable(
    const std::string& name, 
    const std::pair<std::string, std::string>& primaryKey,  
    const std::vector<std::pair<std::string, std::string>>& entries) 
{
    std::stringstream stm;
    stm << "CREATE TABLE IF NOT EXISTS ";
    stm << name;
    
    stm << " (";
    stm << primaryKey.first 
        << primaryKey.second
        << "PRIMARY KEY"
        << ',';
    int idx = 0;
    for (auto&& [name, type] : entries) {
        stm << name << ' ' << type;
        if (idx < entries.size() - 1) {
            stm << ',';
        }
    }
    stm << " )";
    auto rows = con_.exec(stm.str());
}


} // namespace sqlite3_wrapper