#pragma once

#include "irequest_handler.hpp"
#include "server.hpp"

#include "icontroller.hpp"
#include "isubject.hpp"
#include "event.hpp"

namespace controller {

class TODODRequestHandler : 
    public http_server::request_handler::IRequestHandler 
    , public controller::IController
    , public subject::ISubject
{
public: // IRequestHandler  
    void handle(const http_server::Request& req, http_server::Reply& rep) override;

public: // IController
    const todo::ToDoItem& receiveItem() const override; 
    void sendItem(const todo::ToDoItem& item) override;

public: // ISubject interface
    void attach(
        std::shared_ptr<observer::IObserver> obs) override;
    void dettach(
        std::shared_ptr<observer::IObserver> obs) override;

protected: // ISubject interface
    void notify(int newState) override;

private:
    void notifyObservers_(event::event_t ev);
    void handleGetAllTodos_(
        const http_server::Request& req, http_server::Reply& rep);
    void handleAddTodo_(
        const http_server::Request& req, http_server::Reply& rep);

private:
    todo::ToDoItem received_;
    std::vector<todo::ToDoItem> forSend_;
};

} // namespace controller