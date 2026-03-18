#include "todod_request_handler.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include "header.hpp"

namespace {
    using jsonn = nlohmann::json;
    using Rep = http_server::reply::Reply;
    using Header = http_server::header::Header;
} // namespace

namespace controller {

TODODRequestHandler::TODODRequestHandler(http_server::Server& server) {
    using namespace http_server;

    SERVER_ROUTE(server, "/todos/all").method(method::Method::GET)
    ([this](const request::Request& req, reply::Reply& rep){ handleGetAllTodos_(req, rep); });

    SERVER_ROUTE(server, "/todos").method(method::Method::POST)
    ([this](const request::Request& req, reply::Reply& rep){ handleAddTodo_(req, rep); });

    SERVER_ROUTE(server, "/todos").method(method::Method::PATCH)
    ([this](const request::Request& req, reply::Reply& rep){ handleChangeTodo_(req, rep); });

    SERVER_ROUTE(server, "/todos").method(method::Method::DELETE)
    ([this](const request::Request& req, reply::Reply& rep){ handleDeleteTodo_(req, rep); });

    SERVER_ROUTE(server, "/todos").method(method::Method::GET)
    ([this](const request::Request& req, reply::Reply& rep){ handleGetConcreteTodos_(req, rep); });
}

void TODODRequestHandler::setRepo(
    std::shared_ptr<repository::Repository> repo) {
    repo_ = repo;
}

const todo::ToDoItem& TODODRequestHandler::receiveItem() const {
    return received_;
}

void TODODRequestHandler::sendItem(const todo::ToDoItem& item) {
    forSend_.push_back(item);
}

const std::set<std::string>& TODODRequestHandler::changes() const {
    return changes_;
}

const filter::Filter& TODODRequestHandler::filter() const {
    return filter_;
}

void TODODRequestHandler::attach(std::shared_ptr<observer::IObserver> obs) {
    subject::ISubject::attach(obs);
}

void TODODRequestHandler::dettach(std::shared_ptr<observer::IObserver> obs) {
    subject::ISubject::dettach(obs);
}

void TODODRequestHandler::notify(int ev) {
    subject::ISubject::notify(ev);
}

void TODODRequestHandler::update(int newState) {
    auto st = static_cast<event::event_t>(newState);
    switch (st) {
        case event::event_t::ERROR:
            errMsg_ = repo_.lock()->errMsg();
            break;
    }
}

void TODODRequestHandler::notifyObservers_(event::event_t ev) {
    int event = static_cast<int>(ev);
    notify(event);
}

void TODODRequestHandler::handleGetAllTodos_(const http_server::request::Request& req,
                                             http_server::reply::Reply& rep) {
    jsonn jsonRes;
    notifyObservers_(event::event_t::USER_ASK_ALL_TODOS);
    jsonRes["items"] = itemsToJsonNClear_();

    if (errMsg_.empty()) {
        rep.setStatus(http_server::reply::Status::OK);
        rep.setContent(jsonRes.dump());
    } else {
        rep.setStatus(
            http_server::reply::Status::BAD_REQUEST);
        jsonn err;
        err["err"] = true;
        err["err_msg"] = std::move(err);
        rep.setContent(err.dump());
        errMsg_.clear();
    }

    rep.addHeader({http_server::header::key::CONTENT_TYPE,
                    http_server::header::mime_types::JSON});
    rep.addHeader({http_server::header::key::CONTENT_LEN,
                    std::to_string(rep.contentSize())});
}

void TODODRequestHandler::handleAddTodo_(const http_server::request::Request& req,
                                         http_server::reply::Reply& rep) {
    bool hasHeaderJson = false;

    for (auto&& h : req.headers()) {
        if (h.name == http_server::header::key::CONTENT_TYPE &&
            h.value == http_server::header::mime_types::JSON) {
            hasHeaderJson = true;
            break;
        }
    }

    bool ok = false;
    if (hasHeaderJson) {
        auto json = jsonn::parse(req.content(), nullptr, false, true);
        if (!json.is_discarded()) {
            if (json.contains("item")) {
                auto item = json["item"];
                if (item.is_object()) {
                    bool containsAll =
                        item.contains(repository::NAME_COL_NAME) &
                        item.contains(repository::DESC_COL_NAME) &
                        item.contains(repository::TERM_COL_NAME);
                    if (containsAll) {
                        auto name = item[repository::NAME_COL_NAME];
                        auto desc = item[repository::DESC_COL_NAME];
                        auto term = item[repository::TERM_COL_NAME];
                        received_.clear();
                        if (name.is_string() && 
                            desc.is_string() &&
                            term.is_string() &&
                            received_.setTermFromISOString(std::move(term))) 
                        {
                            received_.setName(std::move(name));
                            received_.setDescription(std::move(desc));
                            notifyObservers_(
                                event::event_t::USER_ASK_ADD_TODO);
                            ok = true;
                        } 
                    } 
                }
            }
        }
    }

    if (ok && errMsg_.empty()) {
        rep.setStatus(http_server::reply::Status::OK);
        jsonn ok;
        ok["err"] = false;
        ok["err_msg"] = "OK";
        rep.setContent(ok.dump());
    } else {
        rep.setStatus(
            http_server::reply::Status::BAD_REQUEST);
        jsonn err;
        err["err"] = true;
        err["err_msg"] = ok ? std::move(err) : "Incorect Data Format";
        rep.setContent(err.dump());
    }

    rep.addHeader({http_server::header::key::CONTENT_TYPE,
                    http_server::header::mime_types::JSON});
    rep.addHeader({http_server::header::key::CONTENT_LEN,
                    std::to_string(rep.contentSize())});
}

void TODODRequestHandler::handleChangeTodo_(const http_server::request::Request& req,
                                            http_server::reply::Reply& rep) {

    bool hasHeaderJson = false;

    for (auto&& h : req.headers()) {
        if (h.name == http_server::header::key::CONTENT_TYPE &&
            h.value == http_server::header::mime_types::JSON) {
            hasHeaderJson = true;
            break;
        }
    }
    bool ok = false;
    if (hasHeaderJson) {
        auto json = jsonn::parse(req.content(), nullptr, false, true);
        if (!json.is_discarded()) {
            if (json.contains("item")) {
                auto item = json["item"];
                if (item.is_object()) {
                    int id = -1;
                    std::string name;
                    std::string desc;
                    std::string term;
                    bool completed = false;
                    bool incorrectFormat = false;

                    if (item.contains(repository::ID_COL_NAME)) {
                        auto i = item[repository::ID_COL_NAME];
                        if (i.is_number_integer()) {
                            id = i;
                        } else {
                            incorrectFormat = true;
                        }
                    }
                    if (item.contains(repository::NAME_COL_NAME)) {
                        auto str = item[repository::NAME_COL_NAME];
                        if (str.is_string()) {
                            name = str;
                            changes_.insert(repository::NAME_COL_NAME);
                        } else {
                            incorrectFormat = true;
                        }
                    }
                    if (item.contains(repository::DESC_COL_NAME)) {
                        auto str = item[repository::DESC_COL_NAME];
                        if (str.is_string()) {
                            desc = str;
                            changes_.insert(repository::DESC_COL_NAME);
                        } else {
                            incorrectFormat = true;
                        }
                    }
                    if (item.contains(repository::TERM_COL_NAME)) {
                        auto str = item[repository::TERM_COL_NAME];
                        if (str.is_string()) {
                            term = str;
                            changes_.insert(repository::TERM_COL_NAME);
                        } else {
                            incorrectFormat = true;
                        }
                    }
                    if (item.contains(repository::COMPLETED_COL_NAME)) {
                        auto b = item[repository::COMPLETED_COL_NAME];
                        if (b.is_boolean()) {
                            completed = b;
                            changes_.insert(repository::COMPLETED_COL_NAME);
                        } else {
                            incorrectFormat = true;
                        }
                    }
                    
                    if (!incorrectFormat) {
                        received_.setId(id);
                        received_.setName(std::move(name));
                        received_.setDescription(std::move(desc));
                        received_.setTermFromISOString(std::move(term));
                        received_.setCompleted(completed);
                        notifyObservers_(event::event_t::USER_ASK_CHANGE_TODO);
                        ok = true;
                        changes_.clear();
                        received_.clear();
                    }
                }
            }
        }
    }

    if (ok && errMsg_.empty()) {
        rep.setStatus(http_server::reply::Status::OK);
        jsonn ok;
        ok["err"] = false;
        ok["err_msg"] = "OK";
        rep.setContent(ok.dump());
    } else {
        rep.setStatus(
            http_server::reply::Status::BAD_REQUEST);
        jsonn err;
        err["err"] = true;
        err["err_msg"] = errMsg_.empty() ? "Incorect Data Format" : std::move(errMsg_);
        rep.setContent(err.dump());
        errMsg_.clear();
    }

    rep.addHeader({http_server::header::key::CONTENT_TYPE,
                    http_server::header::mime_types::JSON});
    rep.addHeader({http_server::header::key::CONTENT_LEN,
                    std::to_string(rep.contentSize())});
}

void TODODRequestHandler::handleDeleteTodo_(const http_server::request::Request& req,
                                            http_server::reply::Reply& rep) {
    bool hasHeaderJson = false;

    for (auto&& h : req.headers()) {
        if (h.name == http_server::header::key::CONTENT_TYPE &&
            h.value == http_server::header::mime_types::JSON) {
            hasHeaderJson = true;
            break;
        }
    }

    bool ok = false;
    if (hasHeaderJson) {
        auto json = jsonn::parse(req.content(), nullptr, false, true);
        if (!json.is_discarded()) {
            if (json.contains("item")) {
                auto item = json["item"];
                if (item.is_object()) {
                    if (item.contains(repository::ID_COL_NAME) &&
                        item[repository::ID_COL_NAME].is_number_integer()) 
                    {
                        received_.setId(item[repository::ID_COL_NAME]);
                        notifyObservers_(
                            event::event_t::USER_ASK_DELETE_TODO);
                        ok = true;
                    }
                }
            }
        } 
    }
    if (ok && errMsg_.empty()) {
        rep.setStatus(http_server::reply::Status::OK);
        jsonn ok;
        ok["err"] = false;
        ok["err_msg"] = "OK";
        rep.setContent(ok.dump());
    } else {
        rep.setStatus(
            http_server::reply::Status::BAD_REQUEST);
        jsonn err;
        err["err"] = true;
        err["err_msg"] = ok ? std::move(errMsg_) : "Incorect Data Format";
        rep.setContent(err.dump());
        errMsg_.clear();
    }

    rep.addHeader({http_server::header::key::CONTENT_TYPE,
                    http_server::header::mime_types::JSON});
    rep.addHeader({http_server::header::key::CONTENT_LEN,
                    std::to_string(rep.contentSize())});
}

void TODODRequestHandler::handleGetConcreteTodos_(
    const http_server::request::Request& req,
    http_server::reply::Reply& rep)
{
    bool hasHeaderJson = false;
    filter_.clear();

    for (auto&& h : req.headers()) {
        if (h.name == http_server::header::key::CONTENT_TYPE &&
            h.value == http_server::header::mime_types::JSON) {
            hasHeaderJson = true;
            break;
        }
    }

    bool ok = false;
    jsonn jsonRes;

    if (hasHeaderJson) {
        auto json = jsonn::parse(req.content(), nullptr, false, true);
        if (!json.is_discarded()) {
            if (json.contains("filter")) {
                auto filter = json["filter"];
                if (filter.is_object()) {
                    if (filter.contains(repository::DATE_COL_NAME)) {
                        auto date = filter[repository::DATE_COL_NAME];
                        if (date.is_string()) {
                            filter_.date = date;
                            ok = true;
                            notifyObservers_(event::event_t::USER_ASK_CONCRETE_TODOS);
                            jsonRes["items"] = itemsToJsonNClear_();
                        } 
                    } 
                }
            }
        }
    }

    if (ok && errMsg_.empty()) {
        rep.setStatus(http_server::reply::Status::OK);
        rep.setContent(jsonRes.dump());
    } else {
        rep.setStatus(
            http_server::reply::Status::BAD_REQUEST);
        jsonn err;
        err["err"] = true;
        err["err_msg"] = errMsg_.empty() ? "Incorect Data Format" : std::move(errMsg_);
        rep.setContent(err.dump());
        errMsg_.clear();
    }

    rep.addHeader({http_server::header::key::CONTENT_TYPE,
                    http_server::header::mime_types::JSON});
    rep.addHeader({http_server::header::key::CONTENT_LEN,
                    std::to_string(rep.contentSize())});
}

std::vector<nlohmann::json> TODODRequestHandler::itemsToJsonNClear_() {
    std::vector<jsonn> items;
    for (auto&& i : forSend_) {
        jsonn item;
        item[repository::ID_COL_NAME] = i.id();
        item[repository::NAME_COL_NAME] = i.name();
        item[repository::DESC_COL_NAME] = i.description();
        item[repository::TERM_COL_NAME] = i.termAsISOString();
        item[repository::COMPLETED_COL_NAME] = i.completed();
        items.push_back(std::move(item));
    }
    forSend_.clear();
    return items;
}

}  // namespace controller
