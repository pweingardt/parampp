#include <iostream>
#include "parampp.h"

int main(int argc, char** argv) {
    parampp::Parameters p;
    try {
        p << parampp::Option("h", "help", parampp::OPTIONAL, parampp::NO_ARGS, "Show this help")
            << parampp::Option("v", "version", parampp::OPTIONAL, parampp::NO_ARGS, "Show the version")
            << parampp::Option("f", "file", parampp::OPTIONAL, parampp::SINGLE_ARG, "the configuration file");
        p.parse(argc, argv);
    } catch(parampp::ParameterException e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    std::cout << "File: " << p.get("file") << std::endl;
    std::cout << "print help: " << p.getFlag("help") << std::endl;
    std::cout << "print version: " << p.getFlag("version") << std::endl;

    if(p.getFlag("help")) {
        p.printUsage();
    }
}
