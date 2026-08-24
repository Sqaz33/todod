#pragma once

#include <memory>
#include <vector>

#include "domain/script.hpp"
#include "domain/task.hpp"
#include "scripting/script_engine.hpp"
#include "database/todo_repository.hpp"

namespace todod::service {

class ScriptService {
public:
    ScriptService(
        std::shared_ptr<scripting::ScriptEngine> engine,
        std::shared_ptr<repository::TodoRepository> todoRepo
    );

public:
    void runHandlers(const TodoTask& todo, TodoEvent event);

private:
    void executeCommands_(const scripting::ExecutionResult& res);

private:
    std::shared_ptr<scripting::ScriptEngine> engine;
    std::shared_ptr<repository::TodoRepository> todoRepo;
};

} // namespace todod::service