#pragma once

#include <vector>
#include <optional>
#include <utility>
#include <expected>
#include <cstdint>

#include "domain/todo.hpp"
#include "infrastructure/database/database.hpp"
#include "infrastructure/database/sqlite_statement_guard.hpp"
#include "storage_error.hpp"


namespace todod::repository {

using TaskOrError = std::expected<
    domain::TodoTask, 
    db::error::StorageError
>;

using GetAllResult = std::expected<
    std::vector<domain::TodoTask>,
    db::error::StorageError
>;

using MaybeError = std::optional<db::error::StorageError>; 

using GetPageResult = std::expected<
    domain::TodoPage,
    db::error::StorageError
>;

using GetCountResult = std::expected<
    std::int32_t,
    db::error::StorageError
>;

} // namespace todod::repository