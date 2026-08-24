#include "script_api.hpp"

namespace {

bool validateLua(HandlerScript& script) {
    sol::state lua;
    auto&& loaded = lua.load(
        script.
    ) 
}

bool validateExecution(HandlerScript& script) {

}

} // namespace

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
        "titile", sol::readonly(&ScriptTodo::title),
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


void validateScript(HandlerScript& script) {
    return validateLua(script) && validateExecution(script);
}  

} // namespace todod::scripting::api 
