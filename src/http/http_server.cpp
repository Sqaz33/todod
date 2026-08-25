#include "http_server.hpp"

#include "todo_routes.hpp"
#include "script_routes.hpp"
#include "docs_routes.hpp"

namespace todod::http {

HttpServer::HttpServer(    
    std::shared_ptr<service::TodoService> todoService,
    std::shared_ptr<service::ScriptService> scriptService,
    int port,
    int threads) :
    todoService_(todoService)
    , scriptService_(scriptService)
    , port_(port)
    , threads_(threads)
{
    http::routes::registerTodoRoutes(crowApp_, *todoService_);
    http::routes::registerScriptRoutes(crowApp_, *scriptService_);
    http::routes::registerDocsRoutes(crowApp_);
}

void HttpServer::run() {
    crowApp_//.bindaddr("127.0.0.1")
    .port(port_).concurrency(threads_).run();
}

} // namespace todod::http
