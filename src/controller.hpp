#pragma once

#include "icontroller.hpp"
#include "isubject.hpp"
#include "event.hpp"

namespace controller {

class CLIControoler : 
    public IController
    , public subject::ISubject
{
public:
    void run();

public: // IController interface
    const todo::ToDoItem& receiveItem() const override;
    void sendItem(const todo::ToDoItem& item) override;

private:
    void notifyObservers_(event::event_t ev);

public: // ISubject interface
    void attach(
        std::shared_ptr<observer::IObserver> obs) override;
    void dettach(
        std::shared_ptr<observer::IObserver> obs) override;

protected: // ISubject interface
    void update(int newState) override;

private:
    todo::ToDoItem last_;
};

}