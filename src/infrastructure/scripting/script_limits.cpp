#include "script_limits.hpp"

namespace todod::scripting::limits {

void executionLimitHook(lua_State* lua, lua_Debug*) {
    auto** storage =
        static_cast<ScriptBudget**>(lua_getextraspace(lua));

    ScriptBudget* budget = *storage;
    budget->instructionsLeft -= HOOK_INTERVAL;

    const bool instructionLimitReached =
        budget->instructionsLeft <= 0;

    const bool timeoutReached =
        std::chrono::steady_clock::now() >= budget->deadline;

    if (instructionLimitReached || timeoutReached) {
        luaL_error(lua, "Lua execution limit exceeded");
    }
}

ScriptExecutionLimit::ScriptExecutionLimit(
    sol::state& lua,
    std::chrono::milliseconds timeout,
    std::int64_t instructionLimit)
    : lua_(lua.lua_state())
    , budget_{
        std::chrono::steady_clock::now() + timeout,
        instructionLimit
    }
{
    auto** storage =
        static_cast<ScriptBudget**>(lua_getextraspace(lua_));

    *storage = &budget_;

    lua_sethook(
        lua_,
        executionLimitHook,
        LUA_MASKCOUNT,
        HOOK_INTERVAL
    );
}

ScriptExecutionLimit::~ScriptExecutionLimit() {
    lua_sethook(lua_, nullptr, 0, 0);

    auto** storage =
        static_cast<ScriptBudget**>(lua_getextraspace(lua_));

    *storage = nullptr;
}

} // namespace todod::scripting::limits