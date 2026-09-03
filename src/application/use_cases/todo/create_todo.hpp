#pragma once

#include <variant>

#include "domain/todo.hpp"
#include "application/ports/repository/storage_error.hpp"

namespace todod::use_cases::create_todo {

using TodoCreationError =  std::variant<
    domain::TodoValidationError, 
    db::error::StorageError
>;

struct CreateTodoOutput {
    domain::TodoTask todo;
    HandlerReport handler;
};

using TodoCreationResult = std::expected<CreateTodoOutput, TodoCreationError>;

TodoCreationResult createTodo(const domain::TodoInput& input);

} // namespace todod::use_cases::create_todo