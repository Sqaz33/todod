#include "application.hpp"

#include "http/http_server.hpp"

namespace todod::app {

TododApp::TododApp() {
    auto db = std::make_shared<db::DataBase>("./data.db");
    auto todoRepo = std::make_shared<repository::TodoRepository>(db);
    auto scriptRepo = std::make_shared<repository::ScriptRepository>(db);
    auto scirptEngine = std::make_shared<scripting::ScriptEngine>();
    auto scriptService = std::make_shared<service::ScriptService>(scirptEngine, scriptRepo, todoRepo);
    auto todoService = std::make_shared<service::TodoService>(todoRepo, scriptService);
    auto server = std::make_shared<http::HttpServer>(todoService, scriptService, 8000, 2);
    server->run();
}


} // namespace todod::app
