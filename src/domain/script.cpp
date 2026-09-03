#include "script.hpp"

#include <limits>

#include "limits.hpp"
#include "time/iso8601_helper.hpp"

namespace todod::domain {

HandlerScriptDefinition HandlerScriptDefinition::create(const HandlerScriptInput& input) {
    if (input.name.empty()) {
        return std::unexpected(HandlerScriptValidationError::EmptyName);
    }

    if (input.name.length() > limits::HandlerNameMaxBytes) { 
        return std::unexpected(HandlerScriptValidationError::NameTooLong);
    }

    if (input.source.length() > limits::TodoDescriptionMaxBytes) {
        return std::unexpected(HandlerScriptValidationError::SourceTooLong);
    }

    if (input.event < 0) {
        return std::unexpected(HandlerScriptValidationError::NegativeEvent);
    }

    if (input.event > std::numeric_limits<int>::max()) {
        return std::unexpected(HandlerScriptValidationError::UnkownEvent);
    }

    if (!ALL_EVENTS.contains(static_cast<TodoEvent>(input.event))) {
        return std::unexpected(HandlerScriptValidationError::UnkownEvent);
    }

    return HandlerScriptDefinition(
        input.name,
        input.source,
        static_cast<TodoEvent>(input.event),
        input.enabled
    );
}

HandlerScriptDefinition HandlerScriptDefinition::rehydrate(
    const std::string& name,
    const std::string& source,
    TodoEvent event,
    bool enabled,
) {
    return TodoDefinition(
        name,
        source,
        event,
        enabled
    );
}

HandlerScriptDefinition::HandlerScriptDefinition(
    const std::string& name,
    const std::string& source,
    TodoEvent event,
    bool enabled) :
    name_(name)
    , source_(source)
    , event_(event)
    , enabled_(enabled)
{}

const std::string& HandlerScriptDefinition::name() const noexcept {
    return name_;
}

const std::string& HandlerScriptDefinition::source() const noexcept {
    return source_;
}

TodoEvent HandlerScriptDefinition::event() const noexcept {
    return event_;
}

bool HandlerScriptDefinition::enabled() const noexcept {
    return enabled_;
}

} // namespace todod::domain