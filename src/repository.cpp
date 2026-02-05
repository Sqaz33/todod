#include "repository.hpp"

#include <algorithm>
#include <iterator>

#include "event.hpp"

namespace repository {

Repository::Repository(const std::filesystem::path& dbFile,
                       std::shared_ptr<controller::IController> controller)
    : db_(dbFile), contr_(controller) {
    db_.createTable(TABLE_NAME, {ID_COL_NAME, "INTEGER"},
                    {{NAME_COL_NAME, "TEXT"},
                     {DESC_COL_NAME, "TEXT"},
                     {TERM_COL_NAME, "TEXT"},
                     {COMPLETED_COL_NAME, "BOOLEAN"}},
                    {{COMPLETED_COL_NAME, "FALSE"}});
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
        case event_t::USER_ASK_CHANGE_TODO:
            changeItemFromController_();
            break;
        case event_t::USER_ASK_DELETE_TODO:
            deleteItemFromController_();
            break;
    }
}

void Repository::attach(std::shared_ptr<observer::IObserver> obs) {
    subject::ISubject::attach(obs);
}

void Repository::dettach(std::shared_ptr<observer::IObserver> obs) {
    subject::ISubject::dettach(obs);
}

void Repository::notify(int ev) {
    subject::ISubject::notify(ev);
}

const std::string& Repository::errMsg() const noexcept {
    return errMsg_;
}

void Repository::sendItemsToController_() {
    std::vector<todo::ToDoItem> items;
    auto dbRes =
        db_.getAllRows<int, std::string, std::string, std::string, bool>(
            TABLE_NAME);

    for (auto [id, name, desc, term, comp] : dbRes) {
        items.emplace_back(name, desc, term, id, comp);
    }
    std::for_each(items.begin(), items.end(),
                  [this](auto&& i) { contr_->sendItem(i); });
}

void Repository::addItemFromController_() {
    auto&& item = contr_->receiveItem();
    auto&& name = item.name();
    auto&& desc = item.description();
    auto&& term = item.termAsISOString();
    db_.addRow(TABLE_NAME, {NAME_COL_NAME, DESC_COL_NAME, TERM_COL_NAME},
               {name, desc, term});
}

void Repository::changeItemFromController_() {
    auto&& item = contr_->receiveItem();

    std::vector<std::pair<std::string, std::string>> vals;
    auto&& changes = contr_->changes();

    bool hasChange = false;
    if (changes.contains(NAME_COL_NAME)) {
        hasChange = true;
        vals.push_back({NAME_COL_NAME, item.name()});
    }
    if (changes.contains(DESC_COL_NAME)) {
        hasChange = true;
        vals.push_back({DESC_COL_NAME, item.description()});
    }
    if (changes.contains(TERM_COL_NAME)) {
        hasChange = true;
        vals.push_back({TERM_COL_NAME, item.termAsISOString()});
    }
    std::string b = item.completed() ? "TRUE" : "FALSE";
    if (changes.contains(COMPLETED_COL_NAME)) {
        hasChange = true;
        vals.push_back({COMPLETED_COL_NAME, b});
    }

    if (hasChange) {
        if (item.id() != -1) {
            if (db_.atLeastOneRowExists(
                    TABLE_NAME, {{ID_COL_NAME, std::to_string(item.id())}})) {
                db_.setValuesInRows(TABLE_NAME, vals,
                                    {{ID_COL_NAME, std::to_string(item.id())}});
            } else {
                errMsg_ = "An item with the same ID is missing";
            }

        } else {
            errMsg_ = "Missing id";
        }
    } else {
        errMsg_ = "Nothing to change";
    }

    if (!errMsg_.empty()) {
        notifyObservers_(event::event_t::ERROR);
        errMsg_.clear();
    }
}

void Repository::deleteItemFromController_() {
    auto&& item = contr_->receiveItem();

    if (item.id() != -1) {
        if (db_.atLeastOneRowExists(
                TABLE_NAME, {{ID_COL_NAME, std::to_string(item.id())}})) {
            db_.deleteRows(TABLE_NAME,
                           {{ID_COL_NAME, std::to_string(item.id())}});
        } else {
            errMsg_ = "An item with the same ID is missing";
        }
    } else {
        errMsg_ = "Missing id";
    }

    if (!errMsg_.empty()) {
        notifyObservers_(event::event_t::ERROR);
        errMsg_.clear();
    }
}

void Repository::notifyObservers_(event::event_t ev) {
    int event = static_cast<int>(ev);
    notify(event);
}

}  // namespace repository