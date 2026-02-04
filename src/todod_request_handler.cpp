#include "todod_request_handler.hpp"
#include "header.hpp"

#include <sstream>
#include <vector>
#include <iostream>

#include <nlohmann/json.hpp>

namespace controller {

void TODODRequestHandler::handle(
    const http_server::Request& req, http_server::Reply& rep)
{   
    using Rep = http_server::Reply;
    using jsonn = nlohmann::json;
    using Header = http_server::Header;


    jsonn jsonRes;
    bool hasRes = false;
    auto st = Rep::status_type::bad_request;
    if (req.method() == "GET" && req.uri() == "/todos/all") {
        forSend_.clear();
        notifyObservers_(event::event_t::USER_ASK_ALL_TODOS);      
        std::vector<jsonn> items;
        for (auto&& i : forSend_) {
            jsonn item;
            item["name"] = i.name();
            item["description"] = i.description();
            item["term"] = i.termAsISOString();
#ifdef LOG
            std::cout << "LOG: packaging in json\n";
            std::cout << item << '\n';
#endif // LOG
            items.push_back(std::move(item));
        }
        jsonRes["items"] = std::move(items);

        hasRes = true;
        st = Rep::status_type::ok;
    } else if (req.method() == "POST" && req.uri() == "/todos") {
        bool hasHeaderJson = false;

        for (auto&& h : req.headers()) {
            if (h.name == Header::NAME_CONTENT_TYPE &&
                h.value == Header::VALUE_APPLICATION_JSON) 
            {
                hasHeaderJson = true;
                break;
            } 
        }

        if (hasHeaderJson) {
            auto json = jsonn::parse(req.content(), nullptr, false, true);
            if (!json.is_discarded()) {
                if (json.contains("item")) {
                    auto item = json["item"];
                    if (item.is_object()) {
                        bool containsAll = true;
                        
                        containsAll = 
                            item.contains("name") &
                            item.contains("description") & 
                            item.contains("term");
                        if (containsAll) {
                            auto name = item["name"];
                            auto desc = item["description"];
                            auto term = item["term"];
                            if (name.is_string() && 
                                desc.is_string() && 
                                term.is_string() && 
                                received_.setTermFromISOString(std::move(term))) 
                            {
                                received_.setName(std::move(name));
                                received_.setDescription(std::move(desc));
#ifdef LOG
                                    std::cout << "LOG: received item in json\n";
                                    std::cout << item << '\n';
#endif // LOG
                                notifyObservers_(event::event_t::USER_ASK_ADD_TODO);      
                                st = Rep::status_type::ok;
                            }
                        } 
                    }
                }
            } 
        }  
    }

    if (hasRes) {
        std::string res = jsonRes.dump();
        rep.addHeader( 
            { http_server::Header::NAME_CONTENT_TYPE,
              http_server::Header::VALUE_APPLICATION_JSON }
        );
        rep.addHeader(
            { http_server::Header::NAME_CONTENT_LEN,
              std::to_string(res.size()) }
        );
        rep.setContent(std::move(res));
    } 

    rep.setStatus(st);
}

const todo::ToDoItem& 
TODODRequestHandler::receiveItem() const {
    return received_;
}

void TODODRequestHandler::sendItem(const todo::ToDoItem& item) {
    forSend_.push_back(item);
}

void TODODRequestHandler::notifyObservers_(event::event_t ev) {
    int event = static_cast<int>(ev);
    notify(event);
}

void TODODRequestHandler::notify(int ev) 
{ subject::ISubject::notify(ev); } 

void TODODRequestHandler::attach(
    std::shared_ptr<observer::IObserver> obs) 
{ subject::ISubject::attach(obs); }

void TODODRequestHandler::dettach(
    std::shared_ptr<observer::IObserver> obs) 
{ subject::ISubject::dettach(obs); }

} // namespace controller
