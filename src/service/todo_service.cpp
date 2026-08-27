#include "todo_service.hpp"

namespace todod::service {

TodoService::TodoService(
    std::shared_ptr<repository::TodoRepository> repository,
    std::shared_ptr<ScriptService> scripting) : 
    repository_(repository)
    , scripting_(scripting)
{}

TodoTask TodoService::create(
    std::string title,
    std::string description,
    unsigned priority,
    std::chrono::system_clock::time_point completedAt,
    bool completed) 
{
    auto&& task = repository_->create(
        title, 
        description, 
        priority, 
        completedAt, 
        completed);
    scripting_->runHandlers(task, TodoEvent::ADDED_TODO);

    auto&& newTaskAfterHandlers = repository_->findByID(task.id);
    return newTaskAfterHandlers.value();
}

std::vector<TodoTask> TodoService::getAllTodos() {
    return repository_->getAll();
}

std::optional<TodoTask> TodoService::findByID(std::int64_t id) {
    return repository_->findByID(id);
}

std::optional<TodoPage> 
TodoService::getPage(std::int32_t offset, std::int32_t limit) {
    return repository_->getPage(offset, limit);
}

bool TodoService::updateTodo(const TodoTask& task) {
    return repository_->updateTodo(task);
}

bool TodoService::removeTodo(std::int64_t id) {
    return repository_->removeTodo(id);
}

bool TodoService::completeTodo(std::int64_t id) {
    return repository_->setCompleteStatus(id, true);
}

bool TodoService::uncompleteTodo(std::int64_t id) {
    return repository_->setCompleteStatus(id, false);
}

} // namespace todod::repository