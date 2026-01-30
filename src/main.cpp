#include <memory>
#include <exception>
#include <iostream>
#include <fstream>

#include "controller.hpp"
#include "repository.hpp"

int main() try {
    auto dbFile = "todo.db";

    { std::fstream f(dbFile); }

    auto contr = 
        std::make_shared<controller::CLIControoler>();
    auto rep = 
        std::make_shared<repository::Repository>(dbFile, contr);
    
    contr->attach(rep);

    contr->run();
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
} catch (...) {
    std::cerr << "Unknown error\n";
}