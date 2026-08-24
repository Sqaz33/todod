#pragma once

#include <memory>
#include <vector>

#include "domain/script.hpp"
#include "domain/task.hpp"

namespace todod::service {

class ScriptService {
public:
    void runHandlers(const TodoTask& todo, TodoEvent event);
};

} // namespace todod::service