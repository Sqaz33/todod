#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <vector>
#include <expected>

#include "page.hpp"
#include "ids.hpp"

namespace todod::domain {

struct TodoInput {
    std::string title;
    std::string description;
    std::int64_t priority;
    std::string completedAt;
    bool completed;
};

enum class TodoValidationError {
    EmptyTitle,
    TitleTooLong,
    DescriptionTooLong,
    NegativePriority,
    PriorityTooLarge,
    InvalidCompletedAtFormat
};

class TodoDefinition;

using TodoDefinitionResult = std::expected<TodoDefinition, TodoValidationError>;

class TodoDefinition {
public:
    static TodoDefinitionResult create(const TodoInput& input);  
    static TodoDefinition rehydrate(
        const std::string& title,
        const std::string& description,
        unsigned priority,
        const std::chrono::system_clock::time_point& completedAt,
        bool completed
    );  

public:
    const std::string& title() const noexcept;
    const std::string& description() const noexcept;
    unsigned priority() const noexcept;
    const std::chrono::system_clock::time_point& completedAt() const noexcept;
    bool completed() const noexcept;

private:
    TodoDefinition(       
        const std::string& title,
        const std::string& description,
        unsigned priority,
        const std::chrono::system_clock::time_point& completedAt,
        bool completed);

private:
    std::string title_;
    std::string description_;
    unsigned priority_;
    std::chrono::system_clock::time_point completedAt_;
    bool completed_;
};

struct TodoTask {
    TodoId id;
    TodoDefinition def;
};

using TodoPage = Page<TodoDefinition>;

} // namespace todod::domain