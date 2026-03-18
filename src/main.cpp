#include <exception>
#include <fstream>
#include <iostream>
#include <memory>

#include "server.hpp"
#include "todod_request_handler.hpp"
#include "repository.hpp"

int main(int argc, char** argv) try {
    auto dbFile = "todo.db";

    { std::fstream f(dbFile); }

    // if (argc != 3) {
    //     std::cerr << "Using todod [addres] [port]\n";
    //     return 1;
    // }
    // auto server = http_server::createV10(argv[1], argv[2], contr)
    auto server = http_server::createV10("localhost", "6666");
    auto contr = std::make_shared<controller::TODODRequestHandler>(server);

    auto rep = std::make_shared<repository::Repository>(dbFile, contr);

    contr->attach(rep);
    contr->setRepo(rep);
    rep->attach(contr);

    server.run();
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
} catch (...) {
    std::cerr << "Unknown error\n";
}