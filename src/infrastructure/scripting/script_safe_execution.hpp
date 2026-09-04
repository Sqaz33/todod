#pragma once

#include <vector>
#include <utility>
#include <optional>
#include <type_traits>
#include <tuple>
#include <regex>
#include <cstdint>


#define SOL_ALL_SAFETIES_ON 1
#include "sol.hpp"

#include "script_error.hpp"
#include "script_limits.hpp"

namespace detail__ {

std::optional<std::uint32_t> extractLine(const std::string& text) {
    std::regex pattern(R"(^\[.*\]\:([0-9]*)\:.*$)");
    std::smatch matches;
    if (std::regex_match(text, matches, pattern)) {
        auto&& lineNumberString = matches[1].str();
        return static_cast<std::uint32_t>(std::stoul(lineNumberString));
    }

    return std::nullopt;
}

} // namespace detail__


namespace todod::scripting::execution {

template <class T>
auto named(std::string name, T value) {
    return std::pair<std::string, T>{
        std::move(name),
        value
    };
}    

template <class... TableTy, class... ComTy>
std::optional<error::ScriptError> 
execute(
    const std::string& source,
    const std::string& tableName,
    std::tuple<TableTy...> table,
    std::tuple<ComTy...> commands
) 
{
    using namespace scripting::error;
    using namespace scripting::limits;

    sol::state lua;
    
    auto&& loaded = lua.load(
        source,
        "user_script",
        sol::load_mode::text
    );

    if (!loaded.valid()) {
        auto&& loadCode = [&] {
            switch (loaded.status()) {
                case sol::load_status::syntax:
                    return ScriptErrorCode::SyntaxError;
                case sol::load_status::memory:
                    return ScriptErrorCode::MemoryLimitExceeded;
                default:
                    return ScriptErrorCode::InternalError;
            }
        };

        auto&& diagnostic = lua.get<sol::error>().what();

        return ScriptError{
            .phase = ScriptPhase::Compilation,
            .code = loadCode(),
            .diagnostic = diagnostic,
            .line = detail__::extractLine(diagnostic)
        };
    }

    sol::environment env(lua, sol::create);

    auto&& luaTable = lua.create_table();
    std::apply(
        [&](auto&&... t) { (luaTable[t.first] = t.second, ...); },
        table
    );
    env[tableName] = luaTable;

    std::apply(
        [&](auto&&... com) { (env[com.first] = com.second, ...); },
        commands
    );

    constexpr int timeoutMilliseconds = 100;
    constexpr std::int64_t instructionLimit = 1'000'000;
    limits::ScriptExecutionLimit limit(      
      lua,
      std::chrono::milliseconds{timeoutMilliseconds},
      instructionLimit
    );

    sol::protected_function function = loaded;
    sol::set_environment(env, function);
    auto&& res = function();
    
    if (limit.status() != LimitStatus::NotExceeded) {
        auto&& limitStatusCode = [&] {
            switch (limit.status()) {
                case LimitStatus::InstructionLimitExceeded:
                    return ScriptErrorCode::InstructionLimitExceeded;
                case LimitStatus::TimeLimitExceeded:
                    return ScriptErrorCode::TimeLimitExceeded;
            }
        };

        auto&& diagnostic = lua.get<sol::error>().what();

        return ScriptError{
            .phase = ScriptPhase::Execution,
            .code = limitStatusCode(),
            .diagnostic = diagnostic,
            .line = detail__::extractLine(diagnostic)
        };        
    }

    if (!res.valid()) {
        sol::error error = res;
        auto&& diagnostic = error.what();
        return ScriptError{
            .phase = ScriptPhase::Execution,
            .code = ScriptErrorCode::RuntimeError,
            .diagnostic = diagnostic,
            .line = detail__::extractLine(diagnostic)
        };  
    }

    return std::nullopt;
}

} // namespace todod::scripting::execution