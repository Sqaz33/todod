#pragma once

#include <set>
#include <memory>

#include "iobserver.hpp"
#include "todo_item.hpp"
#include "icontroller.hpp"

namespace repository {

class Repository : public observer::IObserver {
public:
    Repository(
        std::shared_ptr<controller::IController> controller);

public:
    void update(int newState) override;

private:
    void sendItemsToController_();
    void addItemFromController_();

private:
    std::shared_ptr<controller::IController> contr_;
    std::set<todo::ToDoItem> items_;
};

} // namespace repository