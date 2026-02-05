#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ostream>
#include <string>

namespace todo {

class ToDoItem {
   public:
    ToDoItem(std::string name, std::string description,
             boost::posix_time::ptime term, int id = -1,
             bool completed = false);

    ToDoItem(std::string name, std::string description, std::string termISOTime,
             int id = -1, bool completed = false);

    ToDoItem() = default;

   public:
    std::string toString() const;
    bool operator<(const ToDoItem& rhs) const;

    const std::string& name() const noexcept;
    void setName(std::string name);

    const std::string& description() const noexcept;
    void setDescription(std::string description);

    std::string termAsISOString() const;
    bool setTermFromISOString(const std::string& iso);

    int id() const noexcept;
    void setId(int id) noexcept;

    bool completed() const noexcept;
    void setCompleted(bool c) noexcept;

   private:
    int id_;
    std::string name_;
    std::string description_;
    boost::posix_time::ptime term_;
    bool completed_;
};

}  // namespace todo