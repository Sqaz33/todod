#pragma once

#include <variant>

#include "domain/todo.hpp"

namespace todod::use_cases::create_todo {

using TodoCreationError =  std::variant<TodoValidationError, RepositoryError??>;
using TodoCreationResult = std::expected<TodoTask, TodoCreationError>;

TodoCreationResult createTodo(const TodoInput& input);

} // namespace todod::use_cases::create_todo