#pragma once

#include <set>
#include <memory>

#include "iobserver.hpp"
#include "todo_item.hpp"
#include "icontroller.hpp"

#include "sqlite3_wrapper.hpp"

namespace repository {

class Repository : public observer::IObserver {
public:
    Repository(
        const std::filesystem::path& dbFile, 
        std::shared_ptr<controller::IController> controller);

public:
    void update(int newState) override;

private:
    void sendItemsToController_();
    void addItemFromController_();

private:
    sqlite3_wrapper::DB db_;
    std::shared_ptr<controller::IController> contr_;
};

} // namespace repository