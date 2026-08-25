#include "script_routes.hpp"

#include <optional>
#include <string>
#include <charconv>
#include <string_view>
#include <limits>


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

    TodoEvent event;
    switch (json["event"].nt()) {
        case crow::json::num_type::Signed_integer:
            auto i = json["event"].i();
            if (i < std::numeric_limits<int>::min() || 
                i > std::numeric_limits<int>::max()) 
            {
                return false;
            }
            event = static_cast<TodoEvent>(i);
            break;
        case crow::json::num_type::Unsigned_integer:
            auto u = json["event"].u();
            if (u > std::numeric_limits<int>::max()) {
                return false;
            }
            event = static_cast<TodoEvent>(json["event"].u());
            break;
        default:
            return false;
    }

    if (!ALL_EVENTS.contains(event)) {
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
    auto&& enabled = json["enabled"].b();

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
        if (!jsonScript) {
            res.code = 400;
            res.end();
            return;
        }

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
