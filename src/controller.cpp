#include "controller.hpp"

#include <iostream>
#include <regex>
#include <utility>
#include <format>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace {

// DD.MM.YYYY HH:MM 24h format
std::pair<bool, boost::posix_time::ptime> // OK, ptime
dummyFormat(const std::string& str) {
    std::regex pattern(R"((\d\d)\.(\d\d)\.(\d\d\d\d) (\d\d):(\d\d))");
    std::smatch sm;
    if (std::regex_match(str, sm, pattern)) {
        int day = std::stoi(sm[1]);
        int month = std::stoi(sm[2]);
        int year = std::stoi(sm[3]);
        int hour = std::stoi(sm[4]);
        int min = std::stoi(sm[5]);
        boost::posix_time::time_duration td(hour, min, 0);
        boost::gregorian::date date(year, month, day);
        return {
            true,
            boost::posix_time::ptime(date, td)
        };
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
            notifyObservers_(event::event_t::USER_ASK_ALL_TODOS);
        } else if ("add" == com) {
            std::cout << "Name: ";
            std::string name;
            std::cin >> name;
            std::cout << "Description: ";
            std::string description;
            std::cin >> description;
            std::cout << "Time: DD.MM.YYYY HH:MM 24h format\n";
            std::string date;
            std::cin >> date;
            std::string time;
            std::cin >> time;
            auto [ok, pt] = dummyFormat(date + ' ' + time);
            if (!ok) {
                std::cout << "ERROR. Format: " 
                             "DD.MM.YYYY HH:MM 24h format\n";
                continue;
            }
            last_ = todo::ToDoItem(name, description, std::move(pt));
            notifyObservers_(event::event_t::USER_ASK_ADD_TODO);
        }
    }
}

const todo::ToDoItem& CLIControoler::receiveItem() const {
    return last_;
}

void CLIControoler::sendItem(const todo::ToDoItem& item) {
    std::cout << item.toString() << '\n';
}

void CLIControoler::notifyObservers_(event::event_t ev) {
    int event = static_cast<int>(ev);
    update(event);
}

void CLIControoler::update(int ev) 
{ subject::ISubject::update(ev); } 

void CLIControoler::attach(
    std::shared_ptr<observer::IObserver> obs) 
{ subject::ISubject::attach(obs); }

void CLIControoler::dettach(
    std::shared_ptr<observer::IObserver> obs) 
{ subject::ISubject::dettach(obs); }

} // namespace controller 