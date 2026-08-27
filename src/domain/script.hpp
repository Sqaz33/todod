#pragma once


#include <cstdint>
#include <string>

#include "event.hpp"

namespace todod {

struct HandlerScript {
    std::int64_t id;
    std::string name;
    std::string source;
    TodoEvent event;
    bool enabled = true;
};

} // namespace todod