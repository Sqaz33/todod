#pragma once

#include <tuple>

#include "script_api.hpp"
#include "domain/script.hpp"
#include "domain/task.hpp"

namespace todod::scripting {

using ExecutionResult = std::tuple<
    bool, // TODO нормальная ошибка исполнения
    std::vector<api::ScriptCommand>, 
    std::vector<std::string>
>;

class ScriptEngine {
public:
    ExecutionResult
    execute(const HandlerScript& script, const TodoTask& todo);
};

} // namespace todod::scripting