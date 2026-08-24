#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <vector>

namespace todod {

struct TodoTask {
    std::int64_t id;
    std::string title;
    std::string description;
    unsigned priority;
    std::chrono::system_clock::time_point completedAt;
    bool completed = false;
};

struct TodoPage {
    std::vector<TodoTask> todos;
    struct Meta {
        std::int32_t total;
        std::int32_t offset;
        std::int32_t limit;
    } meta;
};

} // namespace todod 