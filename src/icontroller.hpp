#pragma once

#include <set>

#include "todo_item.hpp"

namespace controller {

struct IController {
    virtual const todo::ToDoItem& receiveItem() const = 0;
    virtual void sendItem(const todo::ToDoItem& item) = 0;
    virtual ~IController() = default;
};

} // namespace controller