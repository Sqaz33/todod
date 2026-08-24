#include "script_service.hpp"

namespace todod::service {

ScriptService::ScriptService(
    std::shared_ptr<scripting::ScriptEngine> engine,
    std::shared_ptr<repository::ScriptRepository> scriptRepo,
    std::shared_ptr<repository::TodoRepository> todoRepo
) : 
    engine_(engine) 
    , scriptRepo_(scriptRepo)
    , todoRepo_(todoRepo) 
{}

void ScriptService::runHandlers(
    const TodoTask& todo, TodoEvent event)
{
    auto&& scripts = scriptRepo_->findByEvent(event);
    for (auto&& script : scripts) {
        auto [ok, commands, _] = engine_->execute(script, todo);
        if (ok) {
            executeCommands_(commands);
        }
    }
}

void ScriptService::executeCommands_(
    const std::vector<scripting::api::ScriptCommand>& commands)
{
    using namespace scripting::api;
    // TODO: ошибка испольнения комманды (нет id)
    for (auto&& command : commands) {
        if (auto* setPriority = std::get_if<SetTodoPriorityCommand>(&command)) {
            todoRepo_->setPriority(setPriority->id, setPriority->priority);
        } else if (auto* complete = std::get_if<CompleteTodoCommand>(&command)) {
            todoRepo_->setCompleteStatus(complete->id, true);
        }
    }
}

} // namespace todod::service