#pragma once

#include <cstdint>
#include <string>

#include "event.hpp"

namespace todod::domain {

struct HandlerScriptInput {
    std::string name;
    std::string source;
    std::int64_t event;
    bool enabled;
};

enum class HandlerScriptValidationError {
    EmptyName,
    NameTooLong,
    SourceTooLong,
    NegativeEvent,
    UnkownEvent,
};

class HandlerScriptDefinition;

using HandlerScriptResult = std::expected<HandlerScriptDefinition, HandlerScript>;

class HandlerScriptDefinition {
public:
    static HandlerScriptDefinition create(const HandlerScriptInput& input);
    static HandlerScriptDefinition rehydrate(
        const std::string& name,
        const std::string& source,
        TodoEvent event,
        bool enabled,
    );

public:
    const std::string& name() const noexcept;
    const std::string& source() const noexcept;
    TodoEvent event() const noexcept;
    bool enabled() const noexcept;

private:
    HandlerScriptDefinition(
        const std::string& name,
        const std::string& source,
        TodoEvent event,
        bool enabled,
    );

private:
    std::string name_;
    std::string source_;
    TodoEvent event_;
    bool enabled_;
};

struct HandlerScript {
    HandlerScriptId id;
    HandlerScriptDefinition def;
};

} // namespace todod::domain