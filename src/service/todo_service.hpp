#pragma once

#include <memory>
#include <vector>

#include "database/todo_repository.hpp"
#include "script_service.hpp"

namespace todod::service {

class TodoService {
public:
    explicit TodoService(
        std::shared_ptr<repository::TodoRepository> repository,
        std::shared_ptr<ScriptService> scripting);

public:
    TodoTask create(
        std::string title,
        std::string description,
        unsigned priority,
        std::chrono::system_clock::time_point completedAt,
        bool completed = false);
    std::vector<TodoTask> getAllTodos();
    std::optional<TodoTask> findByID(std::int64_t id);
    std::optional<TodoPage> getPage(std::int32_t offset, std::int32_t limit);
    bool updateTodo(const TodoTask& task);
    bool removeTodo(std::int64_t id);
    bool completeTodo(std::int64_t id);
    bool uncompleteTodo(std::int64_t id);

private:
    std::shared_ptr<repository::TodoRepository> repository_;
    std::shared_ptr<ScriptService> scripting_;
};

} // namespace todod::service 