#pragma once

#include "event.hpp"
#include "icontroller.hpp"
#include "iobserver.hpp"
#include "irequest_handler.hpp"
#include "isubject.hpp"
#include "repository.hpp"
#include "server.hpp"

namespace controller {

class TODODRequestHandler
    : public http_server::request_handler::IRequestHandler,
      public controller::IController,
      public subject::ISubject,
      public observer::IObserver {
   public:
    void setRepo(std::shared_ptr<repository::Repository> repo);

   public:  // IRequestHandler
    void handle(const http_server::Request& req,
                http_server::Reply& rep) override;

   public:  // IController
    const todo::ToDoItem& receiveItem() const override;
    void sendItem(const todo::ToDoItem& item) override;
    const std::set<std::string>& changes() const override;

   public:  // ISubject interface
    void attach(std::shared_ptr<observer::IObserver> obs) override;
    void dettach(std::shared_ptr<observer::IObserver> obs) override;

   protected:  // ISubject interface
    void notify(int newState) override;

   public:  // IObserver interface
    void update(int newState) override;

   private:
    void notifyObservers_(event::event_t ev);

    void handleGetAllTodos_(const http_server::Request& req,
                            http_server::Reply& rep);
    void handleAddTodo_(const http_server::Request& req,
                        http_server::Reply& rep);
    void handleChangeTodo_(const http_server::Request& req,
                           http_server::Reply& rep);
    void handleDeleteTodo_(const http_server::Request& req,
                           http_server::Reply& rep);

   private:
    todo::ToDoItem received_;
    std::vector<todo::ToDoItem> forSend_;
    std::set<std::string> changes_;
    std::weak_ptr<repository::Repository> repo_;
    std::string errMsg_;
};

}  // namespace controller