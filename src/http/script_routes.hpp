#pragma once

#include "crow_all.h"

#include "service/script_service.hpp"

namespace todod::http::routes {

void registerScriptRoutes(
    crow::SimpleApp& app, 
    service::ScriptService& scriptService);

} // namespace todod::http::routes