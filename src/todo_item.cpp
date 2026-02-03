#include "todo_item.hpp"

#include <sstream>

namespace todo {

ToDoItem::ToDoItem(std::string name,
                   std::string description,
                   boost::posix_time::ptime term):
    name_(std::move(name))
    , description_(std::move(description))
    , term_(std::move(term))
{}

ToDoItem::ToDoItem(std::string name,
                   std::string description,
                   std::string termISOTime) :
    name_(std::move(name))
    , description_(std::move(description))
    , term_(boost::posix_time::from_iso_string(termISOTime))
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

const std::string& ToDoItem::name() const noexcept {
    return name_;
}

void ToDoItem::setName(std::string name) {
    name_.swap(name);
}


const std::string& ToDoItem::description() const noexcept {
    return description_;
}

void ToDoItem::setDescription(std::string description) {
    description_.swap(description);
}

std::string ToDoItem::termAsISOString() const {
    return boost::posix_time::to_iso_string(term_);
}

bool ToDoItem::setTermFromISOString(const std::string& iso) { // TODO !!!!!!!!!
    try {
        term_ = boost::posix_time::time_from_string(iso);
    } catch (...) {
        return false;
    }
    return true;
}

} // namespace todo 