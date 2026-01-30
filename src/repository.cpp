#include "repository.hpp"

#include "event.hpp"

namespace repository {

static const std::string TABLE_NAME = "todos";
static const std::string NAME_COL_NAME = "name";
static const std::string DESC_COL_NAME = "desc";
static const std::string TERM_COL_NAME = "term";

Repository::Repository(
    const std::filesystem::path& dbFile, 
    std::shared_ptr<controller::IController> controller):
    db_(dbFile)
    , contr_(controller)
{
    db_.createTable(TABLE_NAME, 
                    {"id", "INTEGER"}, 
                    {{NAME_COL_NAME, "TEXT"}, 
                     {DESC_COL_NAME, "TEXT"},
                     {TERM_COL_NAME, "TEXT"}});
}

void Repository::update(int newState) {
    using namespace event;
    auto evt = static_cast<event_t>(newState);

    switch (evt) {
        case event_t::USER_ASK_ADD_TODO: 
            addItemFromController_();
            break;
        case event_t::USER_ASK_ALL_TODOS:
            sendItemsToController_();
            break;
    }
} 

void Repository::sendItemsToController_() {
    std::vector<todo::ToDoItem> items;
    auto dbRes = db_.getAllRows<
        int, 
        std::string, 
        std::string, 
        std::string>(TABLE_NAME);

    for (auto [_, name, desc, term] : dbRes) {
        items.emplace_back(name, desc, term);
    }
    std::for_each(items.begin(), items.end(), 
                  [this](auto&& i) { contr_->sendItem(i);});
}

void Repository::addItemFromController_() {
    auto&& item = contr_->receiveItem();
    auto&& name = item.name();
    auto&& desc = item.description();
    auto&& term = item.termAsISOString();
    db_.addRow(TABLE_NAME, 
               {NAME_COL_NAME, 
                DESC_COL_NAME, 
                TERM_COL_NAME}, 
               {name, desc, term});
}

} // namespace repository