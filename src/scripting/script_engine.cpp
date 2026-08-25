#include "script_engine.hpp"

#include "script_api.hpp"
#include "script_limits.hpp"

namespace todod::scripting {

ExecutionResult ScriptEngine::execute(
    const HandlerScript& script, const TodoTask& todo) 
{
    sol::state lua;
    sol::environment env(lua, sol::create);
    
    api::ScriptTodo scriptTodo {
        todo.id, 
        todo.title, 
        todo.completed, 
        todo.priority
    };

    api::ScriptContext ctx;
    api::registerScript(lua, env, scriptTodo, ctx);

    limits::ScriptExecutionLimit limit(      
      lua,
      std::chrono::milliseconds{100},
      1'000'000
    );

    auto&& res = lua.safe_script(
        script.source, 
        env, 
        sol::script_pass_on_error
    );

    return {
        res.valid(),
        std::move(ctx.commands),
        std::move(ctx.logs)
    };
}

} // namespace todod::scripting