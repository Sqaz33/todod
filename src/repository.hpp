#pragma once

#include <memory>
#include <set>

#include "event.hpp"
#include "icontroller.hpp"
#include "iobserver.hpp"
#include "isubject.hpp"
#include "sqlite3_wrapper.hpp"
#include "todo_item.hpp"

namespace repository {

static const std::string TABLE_NAME = "todos";
static const std::string ID_COL_NAME = "id";
static const std::string NAME_COL_NAME = "name";
static const std::string DESC_COL_NAME = "desc";
static const std::string TERM_COL_NAME = "term";
static const std::string COMPLETED_COL_NAME = "completed";

class Repository : public observer::IObserver, public subject::ISubject {
   public:
    Repository(const std::filesystem::path& dbFile,
               std::shared_ptr<controller::IController> controller);

   public:  // IObserver interface
    void update(int newState) override;

   public:  // ISubject interface
    void attach(std::shared_ptr<observer::IObserver> obs) override;
    void dettach(std::shared_ptr<observer::IObserver> obs) override;

   protected:  // ISubject interface
    void notify(int newState) override;

   public:
    const std::string& errMsg() const noexcept;

   private:
    void sendItemsToController_();
    void addItemFromController_();
    void changeItemFromController_();
    void deleteItemFromController_();

    void notifyObservers_(event::event_t ev);

   private:
    sqlite3_wrapper::DB db_;
    std::shared_ptr<controller::IController> contr_;
    std::string errMsg_;
};

}  // namespace repository