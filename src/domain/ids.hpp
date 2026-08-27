#pragma once

#include <cstdint>

namespace todod::domain {

struct TodoId {
    std::int64_t id;
};

struct ScriptId {
    std::int64_t id;    
};

} // namespace todod::domain