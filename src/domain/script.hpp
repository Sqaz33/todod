#pragma once

#include <cstdint>
#include <string>
#include <set>

#define TODO_EVENTS(X) \
    X(ADDED_TODO)

namespace todod {

enum class TodoEvent : int {
#define X(event) event,
    TODO_EVENTS(X)
#undef X
};

extern const std::set<TodoEvent> ALL_EVENTS;

struct HandlerScript {
    std::int64_t id;
    std::string name;
    std::string source;
    TodoEvent event;
    bool enabled = true;
};

} // namespace todod