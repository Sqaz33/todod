#pragma once

#include <set>

#include "todo_item.hpp"
#include "filter.hpp"

namespace controller {

struct IController {
    virtual const todo::ToDoItem& receiveItem() const = 0;
    virtual void sendItem(const todo::ToDoItem& item) = 0;
    virtual const std::set<std::string>& changes() const = 0;
    virtual const filter::Filter& filter() const = 0;
    virtual ~IController() = default;
};

}  // namespace controller