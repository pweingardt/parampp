#include <iostream>
#include "parampp.h"

int main(int argc, char** argv) {
    parampp::Parameters p;
    try {
        p << parampp::Option("h", "help", parampp::OPTIONAL, parampp::NO_ARGS, "Show this help")
            << parampp::Option("v", "version", parampp::OPTIONAL, parampp::NO_ARGS, "Show the version",
                    "1")
            << parampp::Option("f", "file", parampp::REQUIRED, parampp::SINGLE_ARG, "the configuration file",
                    "default.conf")
            << parampp::Option("longf", parampp::OPTIONAL, parampp::NO_ARGS, "No short parameter format "
                    "available")
            << parampp::Option("pid", parampp::REQUIRED, parampp::SINGLE_ARG, "the pid file");
        p.parse(argc, argv);
    } catch(parampp::ParameterException e) {
        std::cout << e.what() << std::endl;
        p.printUsage();
        return 1;
    }

    std::cout << "File: " << p.get("file") << std::endl;
    std::cout << "print help: " << p.getFlag("help") << std::endl;
    std::cout << "print version: " << p.getFlag("version") << std::endl;

    if(p.getFlag("help")) {
        p.printUsage();
    }
}
