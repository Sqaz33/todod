#pragma once

#include <vector>
#include <optional>
#include <utility>
#include <expected>
#include <cstdint>

#include "domain/todo.hpp"
#include "domain/script.hpp"
#include "storage_error.hpp"

namespace todod::repository {

using MaybeError = std::optional<db::error::StorageError>; 

using TaskOrError = std::expected<
    domain::TodoTask, 
    db::error::StorageError
>;

using GetAllResult = std::expected<
    std::vector<domain::TodoTask>,
    db::error::StorageError
>;

using GetTodoPageResult = std::expected<
    domain::TodoPage,
    db::error::StorageError
>;

using GetCountResult = std::expected<
    std::int32_t,
    db::error::StorageError
>;

using UpdateTodoResult = std::expected<
    bool, 
    db::error::StorageError    
>;

using HandlerScriptOrError = std::expected<
    domain::HandlerScript,
    db::error::StorageError
>;

using FindHandlerScriptByEventResult = std::expected<
    std::vector<domain::HandlerScript>,
    db::error::StorageError
>;

} // namespace todod::repository