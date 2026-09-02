#pragma once

#include <chrono>
#include <cstdint>

#include "sol.hpp"

namespace todod::scripting::limits {

constexpr int HOOK_INTERVAL = 10'000;

struct ScriptBudget {
    std::chrono::steady_clock::time_point deadline;
    std::int64_t instructionsLeft;
};

void executionLimitHook(lua_State* lua, lua_Debug*);

class ScriptExecutionLimit {
public:
    ScriptExecutionLimit(
        sol::state& lua,
        std::chrono::milliseconds timeout,
        std::int64_t instructionLimit);

    ~ScriptExecutionLimit();

private:
    lua_State* lua_;
    ScriptBudget budget_;
};

} // namespace todod::scripting::limits