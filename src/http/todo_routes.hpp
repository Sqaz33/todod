#pragma once

#include "crow_all.h"

#include "service/todo_service.hpp"

namespace todod::http::routes {

void registerTodoRoutes(
    crow::SimpleApp& app, 
    service::TodoService& todoService);

} // namespace todod::http::routes