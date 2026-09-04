#include "script_engine.hpp"

#include "infrastructure/scripting/script_safe_execution.hpp"

namespace todod::scripting::engine {

ExecutionResult ScriptEngine::execute(
    const domain::HandlerScript& script, 
    const domain::TodoTask& todo)
{
    api::ScriptContext context;

    auto&& complete = [&context](std::int64_t id) {
        context.commands.push_back(api::CompleteTodoCommand{id});
    };

    auto&& set_priority = [&context](std::int64_t id, int priority) {
        context.commands.push_back(api::SetTodoPriorityCommand{id, priority});
    };

    auto&& log = [&context](const std::string& msg) {
        context.logs.push_back(msg);
    };

    auto mbError = execution::execute(
        script.def.source(),
        "todo",
        std::tuple{
            execution::named("id", todo.id.id),
            execution::named("title", todo.def.title()),
            execution::named("description", todo.def.description()),
            execution::named("priority", todo.def.priority()),
            execution::named("completed", todo.def.completed())
        },
        std::tuple{
            execution::named("complete", complete),
            execution::named("set_priority", set_priority),
            execution::named("log", log)
        }
    );

    return ExecutionResult{
        .context = std::move(context),
        .mbError = std::move(mbError)
    };
}

} // namespace todod::scripting::engine