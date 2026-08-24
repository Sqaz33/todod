#pragma once

#include <cstdint>
#include <string>

namespace todod {

enum class TodoEvent : int {
    ADDED_TODO
};

struct HandlerScript {
    std::int64_t id;
    std::string name;
    std::string source;
    TodoEvent event;
    bool enabled = true;
};

} // namespace todod