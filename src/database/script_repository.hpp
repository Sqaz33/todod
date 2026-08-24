#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "database.hpp"
#include "domain/script.hpp"

namespace todod::repository {

class ScriptRepository {
public:
    explicit ScriptRepository(std::shared_ptr<db::DataBase> db);

public:
    HandlerScript create(
        std::string name,
        std::string source,
        TodoEvent event,
        bool enabled = true);
    std::vector<HandlerScript> getAll();
    bool updateTodo(const HandlerScript& script);
    std::optional<HandlerScript> findByID(std::int64_t id);
    bool removeTodo(std::int64_t id);

private:
    template <class... Ty>
    void insert_(Ty&&... args) {
        int idx = 1;
        (insertionQuery_.bind(idx++, std::forward<Ty>(args)), ...);
        insertionQuery_.exec();
        insertionQuery_.reset();
        insertionQuery_.clearBindings();
    }

private:
    std::shared_ptr<db::DataBase> db_;
    SQLite::Statement insertionQuery_;
    SQLite::Statement getAllQuery_;
    SQLite::Statement updateQuery_;
    SQLite::Statement findByIdQuery_;
    SQLite::Statement removeQuery_;
};

} // namespace todod::repository