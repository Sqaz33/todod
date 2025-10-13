#include "controller.hpp"

#include <iostream>
#include <regex>
#include <utility>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "event.hpp"

namespace {

// DD.MM.YYYY HH:MM 24h format
std::pair<bool, boost::posix_time::ptime> 
dummyFormat(const std::string& str) {
    std::regex pattern(R"((\d\d\).(\d\d).(\d\d) (\d\d):(\d\d))");
    std::smatch sm;
    if (std::regex_match(str, sm, pattern)) {
        
    }
    return {false, {}};
}


} // namespace

namespace controller {

void CLIControoler::run() {
    for (;;) {
        std::string com;
        std::cin >> com;
        if ("exit" == com) {
            return;
        } else if ("all" == com) {
            int ev = static_cast<int>(
                event::event_t::USER_ASK_ALL_TODOS);
            update(ev);
        } else if ("add" == com) {
            std::cout << "Name: ";
            std::string name;
            std::cin >> name;
            std::cout << "Description: ";
            std::string description;
            std::cin >> description;
            std::cout << "Time: DD.MM.YY HH:MM 24h format\n";
            std::string time;
            std::cin >> time;
        }
    }
}

const todo::ToDoItem& CLIControoler::receiveItem() const {
    return last_;
}

void CLIControoler::sendItem(const todo::ToDoItem& item) {
    std::cout << item.toString() << '\n';
}


} // namespace controller 