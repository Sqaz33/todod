#include "todo_routes.hpp"

#include <optional>
#include <string>
#include <charconv>
#include <string_view>

#include "time/iso8601_helper.hpp"

namespace todod::http::routes {

namespace {

auto todoToJson(const TodoTask& todo) {
    crow::json::wvalue json;
    json["id"] = todo.id;
    json["title"] = todo.title;
    json["description"] = todo.description;
    json["priority"] = todo.priority;
    json["completedAt"] = helpers::timePointToIso8601(todo.completedAt);
    json["completed"] = todo.completed;
    return json;
}

std::optional<TodoTask> jsonToTodo(const crow::json::rvalue& json) {
    if (!json.has("title") || 
        !json.has("description") || 
        !json.has("priority") || 
        !json.has("completedAt") || 
        !json.has("completed"))
    return std::nullopt;

    std::int64_t id = json.has("id") ? 
                        static_cast<std::int64_t>(json["id"].u()) : 
                        -1;
    return TodoTask {
        id,
        json["title"].s(),
        json["description"].s(),
        static_cast<unsigned>(json["priority"].u()),
        helpers::iso8601ToTimePoint(json["completedAt"].s()),
        json["completed"].b()
    };
}

std::pair<bool, int> intFromChars(std::string_view chars) {
    int value {};
    if (std::from_chars(chars.data(), chars.data() + chars.size(), value).ec == std::errc{}) {
        return {true, value};
    } else {
        return {false, -1};
    }
}

}

void registerTodoRoutes(
    crow::SimpleApp& app, 
    service::TodoService& todoService) 
{
    CROW_ROUTE(app, "/api/todos").
        methods(crow::HTTPMethod::GET)
    ([&todoService] (const crow::request& req, crow::response& res) {
        char* offsetParam = req.url_params.get("offset");
        char* limitParam = req.url_params.get("limit");
        if (!offsetParam || !limitParam) {
            res.code = 400;
            res.end();
            return;
        }

        auto [ok1, offset] = intFromChars(offsetParam);
        auto [ok2, limit] = intFromChars(limitParam);
        if (!ok1 || !ok2) {
            res.code = 400;
            res.end();
            return;
        }

        auto&& page = todoService.getPage(offset, limit);
        if (page != std::nullopt) {
            crow::json::wvalue jsonPage;

            crow::json::wvalue::list todos;
            for (auto&& todo : page.value().todos) {
                todos.push_back(todoToJson(todo));
            }

            jsonPage["todos"] = std::move(todos);
            jsonPage["meta"]["total"] = page.value().meta.total;
            jsonPage["meta"]["offset"] = page.value().meta.offset;
            jsonPage["meta"]["limit"] = page.value().meta.limit;
            res.code = 200;
            res = std::move(jsonPage);
        } else {
            res.code = 416;
        }

        res.end();
    });
}

} // namespace todod::http::routes
