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

TodoTask TodoRepository::create(
    std::string title,
    std::string description,
    unsigned priority,
    std::chrono::system_clock::time_point completedAt,
    bool completed)
{
    std::lock_guard<std::mutex> lk(db_->mutex());

    insert_(
        title,
        description,
        priority,
        helpers::toTimestamp(completedAt),
        completed);

    return TodoTask{
        db_->connection().getLastInsertRowid(),
        std::move(title),
        std::move(description),
        priority,
        completedAt,
        completed};
}

std::vector<TodoTask> TodoRepository::getAll() {
    std::lock_guard<std::mutex> lk(db_->mutex());

    std::vector<TodoTask> todos;
    while (getAllQuery_.executeStep()) {
        todos.push_back(readTask(getAllQuery_));
    }   
    getAllQuery_.reset();
    getAllQuery_.clearBindings();
    return todos;
}

bool TodoRepository::updateTodo(const TodoTask& task) {
    std::lock_guard<std::mutex> lk(db_->mutex());

    updateQuery_.bind(1, task.title);
    updateQuery_.bind(2, task.description);
    updateQuery_.bind(3, task.priority);
    updateQuery_.bind(4, helpers::toTimestamp(task.completedAt));
    updateQuery_.bind(5, task.completed);
    updateQuery_.bind(6, task.id);

    updateQuery_.exec();
    
    bool updated = db_->connection().getChanges() != 0;

    updateQuery_.reset();
    updateQuery_.clearBindings();

    return updated;
}

std::optional<TodoTask> TodoRepository::findByID(std::int64_t id) {
    std::lock_guard<std::mutex> lk(db_->mutex());

    findByIdQuery_.bind(1, id);
    if (!findByIdQuery_.executeStep()) {
        findByIdQuery_.reset();
        findByIdQuery_.clearBindings();
        return std::nullopt;
    }
    TodoTask task = readTask(findByIdQuery_);

    findByIdQuery_.reset();
    findByIdQuery_.clearBindings();

    return task;
}

std::optional<TodoPage> 
TodoRepository::getPage(std::int32_t offset, std::int32_t limit) {
    if (limit < 0 || offset < 0) {
        return std::nullopt;
    }

    std::lock_guard<std::mutex> lk(db_->mutex());

    getPageQeury_.bind(1, static_cast<std::int64_t>(limit));
    getPageQeury_.bind(2, static_cast<std::int64_t>(offset));
    TodoPage page = {{}, {getCountWithNoLock_(), offset, limit}};

    while (getPageQeury_.executeStep()) {
        page.todos.push_back(readTask(getPageQeury_));
    }

    getPageQeury_.reset();
    getPageQeury_.clearBindings();

    return page;
}

bool TodoRepository::removeTodo(std::int64_t id) {
    std::lock_guard<std::mutex> lk(db_->mutex());

    removeQuery_.bind(1, id);

    removeQuery_.exec();

    bool removed = db_->connection().getChanges() != 0;

    removeQuery_.reset();
    removeQuery_.clearBindings();

    return removed;
}

bool TodoRepository::setCompleteStatus(std::int64_t id, bool status) {
    std::lock_guard<std::mutex> lk(db_->mutex());

    setCompleteQuery_.bind(1, status);
    setCompleteQuery_.bind(2, id);

    setCompleteQuery_.exec();
    
    bool updated = db_->connection().getChanges() != 0;

    setCompleteQuery_.reset();
    setCompleteQuery_.clearBindings();

    return updated;
}

bool TodoRepository::setPriority(std::int64_t id, int priority) {
    std::lock_guard<std::mutex> lk(db_->mutex());

    setPriorityQuery_.bind(1, priority);
    setPriorityQuery_.bind(2, id);

    setPriorityQuery_.exec();
    
    bool updated = db_->connection().getChanges() != 0;

    setPriorityQuery_.reset();
    setPriorityQuery_.clearBindings();

    return updated;
}

std::int32_t TodoRepository::getCount() {
    std::lock_guard<std::mutex> lk(db_->mutex());
    return getCountWithNoLock_();
}

std::int32_t TodoRepository::getCountWithNoLock_() {
    getCountQuery_.executeStep();
    std::int32_t count = getCountQuery_.getColumn(0).getInt();
    getCountQuery_.reset();
    getCountQuery_.clearBindings();
    return count;
}

} // namespace todod::repository