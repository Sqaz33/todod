#pragma once

#include <nlohmann/json.hpp>

#include "event.hpp"
#include "icontroller.hpp"
#include "iobserver.hpp"
#include "isubject.hpp"
#include "repository.hpp"
#include "server.hpp"

namespace controller {

class TODODRequestHandler :
      public controller::IController,
      public subject::ISubject,
      public observer::IObserver {
   public:
     TODODRequestHandler(http_server::Server& server);
    
   public:
    void setRepo(std::shared_ptr<repository::Repository> repo);

   public:  // IController
    const todo::ToDoItem& receiveItem() const override;
    void sendItem(const todo::ToDoItem& item) override;
    const std::set<std::string>& changes() const override;
    const filter::Filter& filter() const override;

   public:  // ISubject interface
    void attach(std::shared_ptr<observer::IObserver> obs) override;
    void dettach(std::shared_ptr<observer::IObserver> obs) override;

   protected:  // ISubject interface
    void notify(int newState) override;

   public:  // IObserver interface
    void update(int newState) override;

   private:
    void notifyObservers_(event::event_t ev);
        
    void handleGetAllTodos_(const http_server::request::Request& req,
                            http_server::reply::Reply& rep);
    void handleAddTodo_(const http_server::request::Request& req,
                        http_server::reply::Reply& rep);
    void handleChangeTodo_(const http_server::request::Request& req,
                           http_server::reply::Reply& rep);
    void handleDeleteTodo_(const http_server::request::Request& req,
                           http_server::reply::Reply& rep);
    void handleGetConcreteTodos_(const http_server::request::Request& req,
                        http_server::reply::Reply& rep);

    std::vector<nlohmann::json> itemsToJsonNClear_();

   private:
    todo::ToDoItem received_;
    std::vector<todo::ToDoItem> forSend_;
    std::set<std::string> changes_;
    std::weak_ptr<repository::Repository> repo_;
    std::string errMsg_;
    filter::Filter filter_;
};

}  // namespace controller