#include "todo_repository.hpp"

#include <format>

#include "time/timestamp_helpers.hpp"

namespace {
    const std::string ID_COLUMN = "id";
    const std::string TITLE_COLUMN = "title";
    const std::string DESCRIPTION_COLUMN = "description";
    const std::string PRIORITY_COLUMN = "priority";
    const std::string COMPLETED_AT_COLUMN = "completed_at";
    const std::string COMPLETED_COLUMN = "completed";
    const std::string TABLE_NAME = "todos";

    todod::TodoTask readTask(SQLite::Statement& query) {
        return todod::TodoTask {
            query.getColumn(0).getInt64(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getUInt(),
            todod::helpers::fromTimestamp(query.getColumn(4).getInt64()),
            query.getColumn(5).getInt() != 0
        };
    }
}

namespace todod::repository {
 
TodoRepository::TodoRepository(std::shared_ptr<db::DataBase> db) : 
    db_(db) 
    , insertionQuery_(db_->connection(), std::format(
        "INSERT INTO {} ({}, {}, {}, {}, {}) VALUES (?, ?, ?, ?, ?)", 
        TABLE_NAME,
        TITLE_COLUMN, 
        DESCRIPTION_COLUMN, 
        PRIORITY_COLUMN, 
        COMPLETED_AT_COLUMN, 
        COMPLETED_COLUMN))
    , getAllQuery_(db_->connection(), std::format(
        "SELECT * FROM {}", 
        TABLE_NAME))
    , updateQuery_(db_->connection(), std::format(
        R"(
            UPDATE {}
            SET {} = ?,
                {} = ?,
                {} = ?,
                {} = ?,
                {} = ?
            WHERE {} = ?
        )",
        TABLE_NAME,
        TITLE_COLUMN, 
        DESCRIPTION_COLUMN, 
        PRIORITY_COLUMN, 
        COMPLETED_AT_COLUMN, 
        COMPLETED_COLUMN,
        ID_COLUMN))
    , findByIdQuery_(db_->connection(), std::format(
        "SELECT * FROM {} WHERE {} = ?",
        TABLE_NAME, 
        ID_COLUMN
    ))
    , removeQuery_(db_->connection(), std::format(
        "DELETE FROM {} WHERE {} = ?",
        TABLE_NAME,
        ID_COLUMN
    ))
    , setCompleteQuery_(db_->connection(), std::format(
        "UPDATE {} SET {} = ? WHERE {} = ?",
        TABLE_NAME,
        COMPLETED_COLUMN,
        ID_COLUMN
    ))
    , getPageQeury_(db_->connection(), std::format(
        R"(
            SELECT * 
            FROM {}
            ORDER BY {} DESC
            LIMIT ? 
            OFFSET ?
        )",
        TABLE_NAME,
        ID_COLUMN
    ))
    , getCountQuery_(db_->connection(), std::format(
        "SELECT COUNT(*) FROM {}", 
        TABLE_NAME
    ))
    , setPriorityQuery_(db_->connection(), std::format(
        "UPDATE {} SET {} = ? WHERE {} = ?",
        TABLE_NAME,
        PRIORITY_COLUMN,
        ID_COLUMN
    ))
{}

TaskOrError TodoRepository::create(const domain::TodoDefinition& def) {
    return db_->access([&this](db::DBAccess& access) { return create(def, access); })
}

TaskOrError TodoRepository::create(const domain::TodoDefinition& def, db::DBAccess&) {
    auto err = insert_(
        def.title(),
        def.description(),
        def.priority(),
        helpers::toTimestamp(def.completedAt()),
        def.completed());   

    if (err) {
        return std::unexpected(err.value());
    }

    return domain::TodoTask{{db_->connection().getLastInsertRowid()}, def};
}

// std::vector<TodoTask> TodoRepository::getAll() {
//     std::lock_guard<std::mutex> lk(db_->mutex());

//     std::vector<TodoTask> todos;
//     while (getAllQuery_.executeStep()) {
//         todos.push_back(readTask(getAllQuery_));
//     }   
//     getAllQuery_.reset();
//     getAllQuery_.clearBindings();
//     return todos;
// }

// bool TodoRepository::updateTodo(const TodoTask& task) {
//     std::lock_guard<std::mutex> lk(db_->mutex());

//     updateQuery_.bind(1, task.title);
//     updateQuery_.bind(2, task.description);
//     updateQuery_.bind(3, task.priority);
//     updateQuery_.bind(4, helpers::toTimestamp(task.completedAt));
//     updateQuery_.bind(5, task.completed);
//     updateQuery_.bind(6, task.id);

//     updateQuery_.exec();
    
//     bool updated = db_->connection().getChanges() != 0;

//     updateQuery_.reset();
//     updateQuery_.clearBindings();

//     return updated;
// }

// std::optional<TodoTask> TodoRepository::findByID(std::int64_t id) {
//     std::lock_guard<std::mutex> lk(db_->mutex());

//     findByIdQuery_.bind(1, id);
//     if (!findByIdQuery_.executeStep()) {
//         findByIdQuery_.reset();
//         findByIdQuery_.clearBindings();
//         return std::nullopt;
//     }
//     TodoTask task = readTask(findByIdQuery_);

//     findByIdQuery_.reset();
//     findByIdQuery_.clearBindings();

//     return task;
// }

GetPageResult TodoRepository::getPage(std::int32_t offset, std::int32_t limit) {
    return db_->access([&](db::DBAccess& access) { return getPage(offset, limit, access); });
}

GetPageResult TodoRepository::getPage(std::int32_t offset, std::int32_t limit, db::DBAccess&) {
    db::guard::StatementResetGuard guard { getPageQeury_ };

    try {
        getPageQeury_.bind(1, static_cast<std::int64_t>(limit));
        getPageQeury_.bind(2, static_cast<std::int64_t>(offset));
        TodoPage page = {{}, {getCount(), offset, limit}};

        while (getPageQeury_.executeStep()) {
            page.todos.push_back(readTask(getPageQeury_));
        }

        getPageQeury_.reset();
        getPageQeury_.clearBindings();

        return page;
    } catch (const SQLite::Exception& e) {
        return std::unexpect(db::error::StorageError::create("get page", e));
    }
}

// bool TodoRepository::removeTodo(std::int64_t id) {
//     std::lock_guard<std::mutex> lk(db_->mutex());

//     removeQuery_.bind(1, id);

//     removeQuery_.exec();

//     bool removed = db_->connection().getChanges() != 0;

//     removeQuery_.reset();
//     removeQuery_.clearBindings();

//     return removed;
// }

UpdateTodoResult TodoRepository::setCompleteStatus(std::int64_t id, bool status) {
    return db_->access([&](db::DBAccess& access) { return setCompleteStatus(id, status, access); });
}

UpdateTodoResult TodoRepository::setCompleteStatus(std::int64_t id, bool status, db::DBAccess&) {
    db::guard::StatementResetGuard guard { setCompleteQuery_ };

    try {
        setCompleteQuery_.bind(1, status);
        setCompleteQuery_.bind(2, id);

        setCompleteQuery_.exec();
        
        bool updated = db_->connection().getChanges() != 0;

        setCompleteQuery_.reset();
        setCompleteQuery_.clearBindings();

       return updated;
    } catch (const SQLite::Exception& e) {
        return std::unexpect(db::error::StorageError::create("set complete status", e));
    }
}

UpdateTodoResult TodoRepository::setPriority(std::int64_t id, int priority) {
    return db_->access([&](db::DBAccess& access) { return setPriority(id, priority, access); });
}

UpdateTodoResult TodoRepository::setPriority(std::int64_t id, int priority, db::DBAccess&) {
    db::guard::StatementResetGuard guard { setPriorityQuery_ };

    try {
        setPriorityQuery_.bind(1, priority);
        setPriorityQuery_.bind(2, id);

        setPriorityQuery_.exec();
        
        bool updated = db_->connection().getChanges() != 0;

        setPriorityQuery_.reset();
        setPriorityQuery_.clearBindings();

        return updated;
    } catch (const SQLite::Exception& e) {
        return std::unexpect(db::error::StorageError::create("set priority", e));
    }
}

GetCountResult TodoRepository::getCount() {
    return db_->access([&](db::DBAccess& access) { return getCount(access); });
}

GetCountResult TodoRepository::getCount(db::DBAccess&) {
    db::guard::StatementResetGuard guard { setPriorityQuery_ };

    try {
        getCountQuery_.executeStep();
        std::int32_t count = getCountQuery_.getColumn(0).getInt();
        getCountQuery_.reset();
        getCountQuery_.clearBindings();
        return count;
    } catch (const SQLite::Exception& e) {
        return std::unexpect(db::error::StorageError::create("get count", e));
    }
}

} // namespace todod::repository