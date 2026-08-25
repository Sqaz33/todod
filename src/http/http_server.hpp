#pragma once

#include <memory>

#include "crow_all.h"

#include "service/script_service.hpp"
#include "service/todo_service.hpp"

namespace todod::http {

class HttpServer {
public:
    HttpServer(    
    std::shared_ptr<service::TodoService> todoService,
    std::shared_ptr<service::ScriptService> scriptService,
    int port,
    // localhost
    int threads = 2);

public:
    void run();

private:
    std::shared_ptr<service::TodoService> todoService_;
    std::shared_ptr<service::ScriptService> scriptService_;
    int port_;
    int threads_;
    crow::SimpleApp crowApp_;
};

} // namespace todod::http