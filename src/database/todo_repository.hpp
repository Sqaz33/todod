#pragma once 

#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <chrono>
#include <utility>

#include "domain/task.hpp"
#include "database.hpp"

namespace todod::repository {

class TodoRepository {
public:
    explicit TodoRepository(std::shared_ptr<db::DataBase> db);

public:
    TodoTask create(
        std::string title,
        std::string description,
        unsigned priority,
        std::chrono::system_clock::time_point completedAt,
        bool completed = false);
    std::vector<TodoTask> getAll();
    bool updateTodo(const TodoTask& task);
    std::optional<TodoTask> findByID(std::int64_t id);
    std::optional<TodoPage> getPage(std::int32_t offset, std::int32_t limit);
    bool removeTodo(std::int64_t id);
    bool setCompleteStatus(std::int64_t id, bool status);
    bool setPriority(std::int64_t id, int priority);
    std::int32_t getCount();

private:
    template <class... Ty>
    void insert_(Ty&&... args) {
        int idx = 1;
        (insertionQuery_.bind(idx++, std::forward<Ty>(args)), ...);
        insertionQuery_.exec();
        insertionQuery_.reset();
        insertionQuery_.clearBindings();
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