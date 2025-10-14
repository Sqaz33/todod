#include "todo_item.hpp"

#include <sstream>

namespace todo {

ToDoItem::ToDoItem(std::string name,
                   std::string description,
                   boost::posix_time::ptime term):
    name_(name)
    , description_(description)
    , term_(term)
{}

std::string ToDoItem::toString() const {
    std::stringstream ss;
    ss << "Name: "        << name_ << ' '
       << "Description: " << description_ << ' ' 
       << "Term: "        << term_; 
    return ss.str();
}

bool ToDoItem::operator<(const ToDoItem& rhs) const {
    return term_ < rhs.term_;   
}

} // namespace todo 