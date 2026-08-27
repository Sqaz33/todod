#include "todo.hpp"

#include <limits>

#include "limits.hpp"
#include "time/iso8601_helper.hpp"

namespace todod::domain {

TodoDefinitionResult TodoDefinition::create(const TodoInput& input) {
    if (input.title.empty()) {
        return std::unexpected(TodoValidationError::EmptyTitle);
    }

    if (input.title.length() > limits::TodoTitleMaxBytes) {
        return std::unexpected(TodoValidationError::TitleTooLong);
    }

    if (input.description.length() > limits::TodoDescriptionMaxBytes) {
        return std::unexpected(TodoValidationError::DescriptionTooLong);
    }

    if (input.priority < 0) {
        return std::unexpected(TodoValidationError::NegativePriority);
    }

    if (input.priority > std::numeric_limits<unsigned>::max()) {
        return std::unexpected(TodoValidationError::PriorityTooLarge);
    }
 
    if (!helpers::isValidIso8601(input.completedAt)) {
        return std::unexpected(TodoValidationError::InvalidCompletedAtFormat);
    }

    return TodoDefinition(
        input.title,
        input.description,
        input.priority,
        input.completedAt,
        input.completed
    );
}

static TodoDefinition TodoDefinition::rehydrate(
    const std::string& title,
    const std::string& description,
    unsigned priority,
    const std::chrono::system_clock::time_point& completedAt,
    bool completed
) 
{
    return TodoDefinition(
        title, 
        description, 
        priority, 
        completedAt, 
        completed); 
}

TodoDefinition::TodoDefinition(
    const std::string& title,
    const std::string& description,
    unsigned priority,
    const std::chrono::system_clock::time_point& completedAt,
    bool completed) :
    title_(title)
    , description_(description)
    , priority_(priority)
    , completedAt_(completedAt)
    , completed_(completed)
{}

const std::string& TodoDefinition::title() const noexcept {
    return title_;
}

const std::string& TodoDefinition::description() const noexcept {
    return description_;
}

unsigned TodoDefinition::priority() const noexcept {
    return priority_;
}

const std::chrono::system_clock::time_point& TodoDefinition::completedAt() const noexcept {
    return completedAt_;
}

bool TodoDefinition::completed() const noexcept {
    return completed_;
}

} // namespace todod::domain