#include <memory>
#include <exception>
#include <iostream>
#include <fstream>

#ifdef CLI
#include "controller.hpp"
#else
#include "server.hpp"
#include "todod_request_handler.hpp"
#endif

#include "repository.hpp"

int main(int argc, char** argv) try {
    auto dbFile = "todo.db";

    { std::fstream f(dbFile); }

    auto contr = 
#ifdef CLI
        std::make_shared<controller::CLIControoler>();
#else
        std::make_shared<controller::TODODRequestHandler>();

    // if (argc != 3) {
    //     std::cerr << "Using todod [addres] [port]\n";
    //     return 1;
    // } 
    // auto server = http_server::createV10(argv[1], argv[2], contr);
    auto server = http_server::createV10("localhost", "6666", contr);
#endif

    auto rep = 
        std::make_shared<repository::Repository>(dbFile, contr);
    
    contr->attach(rep);

#ifdef CLI
    contr->run();
#else
    server.run();
#endif
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
} catch (...) {
    std::cerr << "Unknown error\n";
}