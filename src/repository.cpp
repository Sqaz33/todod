#include "repository.hpp"

#include "event.hpp"

namespace repository {

Repository::Repository(
    std::shared_ptr<controller::IController> controller) :
    contr_(controller)
{}

void Repository::update(int newState) {
    using namespace event;
    auto ev = static_cast<event_t>(newState);

    switch (ev) {
        case event_t::USER_ASK_ADD_TODO: 
            addItemFromController_();
            break;
        case event_t::USER_ASK_ALL_TODOS:
            sendItemsToController_();
            break;
    }
} 

void Repository::sendItemsToController_() {
    for (auto&& item : items_) {
        contr_->sendItem(item);
    }
}

void Repository::addItemFromController_() {
    auto&& item = contr_->receiveItem();
    items_.insert(item);
}


} // namespace repository