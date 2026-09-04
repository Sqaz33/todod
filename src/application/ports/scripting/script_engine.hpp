#pragma once

#include <tuple>
#include <expected>

#include "script_api.hpp"
#include "script_error.hpp"
#include "domain/todo.hpp"
#include "domain/script.hpp"

namespace todod::scripting::engine {

struct ExecutionResult {
    api::ScriptContext context;
    std::optional<error::ScriptError> mbError;
};

class ScriptEngine {
public:
    ExecutionResult execute(
        const domain::HandlerScript& script, 
        const domain::TodoTask& todo);
};

} // namespace todod::scripting::engine