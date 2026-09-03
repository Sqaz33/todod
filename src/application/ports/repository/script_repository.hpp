#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "infrastructure/database/database.hpp"
#include "repository_results.hpp"

namespace todod::repository {

class ScriptRepository {
public:
    explicit ScriptRepository(std::shared_ptr<db::DataBase> db);

public:
    HandlerScriptOrError create(const domain::HandlerScriptDefinition& def);
    HandlerScriptOrError create(const domain::HandlerScriptDefinition& def, db::DBAccess&);
    // std::vector<HandlerScript> getAll();
    // bool updateScript(const HandlerScript& script);
    // std::optional<HandlerScript> findByID(std::int64_t id);
    // bool removeScript(std::int64_t id);
    FindHandlerScriptByEventResult findByEvent(domain::TodoEvent event);
    FindHandlerScriptByEventResult findByEvent(domain::TodoEvent event, db::DBAccess&);

private:
    template <class... Ty>
    MaybeError insert_(Ty&&... args) {
        db::guard::StatementResetGuard guard{ insertionQuery_ };
        try {
            int idx = 1;
            (insertionQuery_.bind(idx++, std::forward<Ty>(args)), ...);
            insertionQuery_.exec();
        } catch (const SQLite::Exception& e) {
            return db::error::StorageError::create("insert handler", e);
        }
    }

private:
    std::shared_ptr<db::DataBase> db_;
    SQLite::Statement insertionQuery_;
    SQLite::Statement getAllQuery_;
    SQLite::Statement updateQuery_;
    SQLite::Statement findByIdQuery_;
    SQLite::Statement removeQuery_;
    SQLite::Statement findByEventQuery_;
};

} // namespace todod::repository