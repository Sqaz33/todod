#pragma once 

#include <memory>
#include <string>
#include <chrono>

#include "repository_results.hpp"
#include "database.hpp"

namespace todod::repository {

class TodoRepository {
public:
    explicit TodoRepository(std::shared_ptr<db::DataBase> db);

public:
    TaskOrError create(const domain::TodoDefinition& def) {
        return db_->access([&this](db::DBSession& session) { return create(def, session); })
    }
    TaskOrError create(const domain::TodoDefinition& def, db::DBSession&) {
        return ...
    }

    GetAllResult getAll();
    MaybeError updateTodo(const domain::TodoTask& task);
    TaskOrError findByID(std::int64_t id);
    GetPageResult getPage(std::int32_t offset, std::int32_t limit);
    MaybeError removeTodo(std::int64_t id);
    MaybeError setCompleteStatus(std::int64_t id, bool status);
    MaybeError setPriority(std::int64_t id, int priority);
    GetCountResult getCount();

private:
    template <class... Ty>
    std::optional<db::error::StorageError> insert_(Ty&&... args) {
        try {
            db::guard::StatementResetGuard guard{insertionQuery_};
            int idx = 1;
            (insertionQuery_.bind(idx++, std::forward<Ty>(args)), ...);
            insertionQuery_.exec();
        } catch (const SQLite::Exception& e) {
            return db::error::StorageError::create("insert todo", e);
        }
        return std::nullopt;
    }

    std::int32_t getCountWithNoLock_();
    
private:
    std::shared_ptr<db::DataBase> db_;
    SQLite::Statement insertionQuery_;
    SQLite::Statement getAllQuery_;
    SQLite::Statement updateQuery_;
    SQLite::Statement findByIdQuery_;
    SQLite::Statement removeQuery_;
    SQLite::Statement setCompleteQuery_;
    SQLite::Statement setPriorityQuery_;
    SQLite::Statement getPageQeury_;
    SQLite::Statement getCountQuery_;
};

} // namespace todod::repository