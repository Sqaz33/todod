#include "script_routes.hpp"

#include <optional>
#include <string>
#include <charconv>
#include <string_view>


namespace todod::http::routes {

namespace {

auto handlerScriptToJson(const HandlerScript& script) {
    crow::json::wvalue json;
    json["id"] = script.id;
    json["name"] = script.name;
    json["source"] = script.source;
    json["event"] = static_cast<int>(script.event);
    json["enabled"] = script.enabled;
    return json;
}

bool checkHandlerScriptWithNoIdInJson(const crow::json::rvalue& json) {
    if (!(json.has("name") &&
          json.has("source") &&
          json.has("event") &&
          json.has("enabled")))
    return false;

    if (json["name"].t() != crow::json::type::String) {
        return false;
    }

    if (json["source"].t() != crow::json::type::String) {
        return false;
    }

    if (json["event"].t() != crow::json::type::Number) {
        return false;
    }

    auto&& enabledTy = json["enabled"].t();
    if (enabledTy != crow::json::type::False && 
        enabledTy != crow::json::type::True) 
    {
        return false;
    }

    return true;
}

std::optional<HandlerScript> createHandlerScript(
    const crow::json::rvalue& json, 
    service::ScriptService& service) 
{
    if (!checkHandlerScriptWithNoIdInJson(json)) { 
        return std::nullopt; 
    }

    auto&& name    = json["name"].s();      
    auto&& source  = json["source"].s();  
    auto&& event   = static_cast<TodoEvent>(json["event"].i());    
    auto&& enabled = json["enabled"].i() != 0;

    return service.create(name, source, event, enabled);
}   

} // namespace

void registerScriptRoutes(
    crow::SimpleApp& app, 
    service::ScriptService& scriptService)
{
    CROW_ROUTE(app, "/api/scripts/handlers").
        methods(crow::HTTPMethod::POST)
    ([&scriptService] (const crow::request& req, crow::response& res) {
        auto jsonScript = crow::json::load(req.body);
        if (auto handlerScript = createHandlerScript(jsonScript, scriptService)) {
            res.code = 200;
            res = handlerScriptToJson(handlerScript.value());
        } else {
            res.code = 400;
        }
        res.end();
    });
}

} // namespace todod::http::routes
