#pragma once

#include "sol.hpp"

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "domain/script.hpp"

namespace todod::scripting::api {

struct ScriptTodo {
    std::int64_t id;
    std::string title;
    bool completed;
    int priority;
};

struct SetTodoPriorityCommand {
    std::int64_t id;
    int priority;
};

struct CompleteTodoCommand {
    std::int64_t id;
};

using ScriptCommand = std::variant<
    SetTodoPriorityCommand, 
    CompleteTodoCommand
>;

struct ScriptContext {
    std::vector<ScriptCommand> commands;
    std::vector<std::string> logs;
};

void registerScript(
    sol::state& lua, 
    sol::environment& env, 
    const ScriptTodo& todo,
    ScriptContext& ctx);

void validateScript(HandlerScript& script);

} // namespace todod::scripting::api 