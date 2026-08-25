#include "script_api.hpp"

#include "domain/script.hpp"
#include "script_limits.hpp"

namespace todod::scripting::api {

void registerScript(
    sol::state& lua, 
    sol::environment& env, 
    const ScriptTodo& todo,
    ScriptContext& ctx) 
{
    lua.new_usertype<ScriptTodo>(
        "Todo",
        "id", sol::readonly(&ScriptTodo::id),
        "title", sol::readonly(&ScriptTodo::title),
        "completed", sol::readonly(&ScriptTodo::completed),
        "priority", sol::readonly(&ScriptTodo::priority)
    );

    env["todo"] = todo;

    env["complete"] = [&ctx](std::int64_t id) {
        ctx.commands.push_back(CompleteTodoCommand{id});
    };

    env["set_priority"] = [&ctx](std::int64_t id, int priority) {
        ctx.commands.push_back(SetTodoPriorityCommand{id, priority});
    };

    env["log"] = [&ctx](const std::string& msg) {
        ctx.logs.push_back(msg);
    };
}


// TODO вернуть ошибку
bool validateScript(const HandlerScript& script) {
    sol::state lua;
    
    auto&& loaded = lua.load(
        script.source,
        "user_script",
        sol::load_mode::text
    );

    if (!loaded.valid()) {
        return false;
    }

    sol::environment env(lua, sol::create);

    auto&& todo = lua.create_table();
    todo["id"] = std::int64_t{1};
    todo["title"] = "test";
    todo["description"] = "test";
    todo["priority"] = 1u;
    todo["completed"] = false;

    env["todo"] = todo;

    env["complete"] = [](std::int64_t id) { };
    env["set_priority"] = [](std::int64_t id, int priority) { };
    env["log"] = [](const std::string& msg) { };

    
    limits::ScriptExecutionLimit limit(      
      lua,
      std::chrono::milliseconds{100},
      1'000'000
    );

    auto&& res = lua.safe_script(
        script.source, 
        env, 
        sol::script_pass_on_error);

    return res.valid();
}  

} // namespace todod::scripting::api 
