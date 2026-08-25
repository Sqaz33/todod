#pragma once

#include <memory>
#include <vector>

#include "domain/script.hpp"
#include "domain/task.hpp"
#include "scripting/script_engine.hpp"
#include "database/todo_repository.hpp"
#include "database/script_repository.hpp"

namespace todod::service {

class ScriptService {
public:
    ScriptService(
        std::shared_ptr<scripting::ScriptEngine> engine,
        std::shared_ptr<repository::ScriptRepository> scriptRepo,
        std::shared_ptr<repository::TodoRepository> todoRepo
    );

public:
    void runHandlers(const TodoTask& todo, TodoEvent event);

    std::optional<HandlerScript> create(
        const std::string& name,
        const std::string& source,
        TodoEvent event,
        bool enabled = true);

private:
    void executeCommands_(
        const std::vector<scripting::api::ScriptCommand>& commands);

private:
    std::shared_ptr<scripting::ScriptEngine> engine_;
    std::shared_ptr<repository::ScriptRepository> scriptRepo_;
    std::shared_ptr<repository::TodoRepository> todoRepo_;
};

} // namespace todod::service