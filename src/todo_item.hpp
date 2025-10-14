#pragma once

#include <string>
#include <ostream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace todo {

class ToDoItem {
public:
    ToDoItem(std::string name,
            std::string description,
            boost::posix_time::ptime term);

    ToDoItem() = default;

public:
    std::string toString() const;
    bool operator<(const ToDoItem& rhs) const;

private:
    std::string name_;
    std::string description_;
    boost::posix_time::ptime term_;
};

} // namespace todo 