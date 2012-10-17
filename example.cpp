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
            << parampp::Option("p", "pid", parampp::REQUIRED, parampp::MULTI_ARGS, "the pid files");
        p.parse(argc, argv);
    } catch(parampp::ParameterException e) {
        std::cout << e.what() << std::endl;
        p.printUsage();
        return 1;
    }

    if(p.getFlag("help")) {
        p.printUsage();
        return 0;
    }

    try {
        p.checkRequired();
    } catch(parampp::ParameterException e) {
        std::cout << e.what() << std::endl;
        p.printUsage();
        return 1;
    }

    std::cout << "File: " << p.get("file") << std::endl;
    std::cout << "print help: " << p.getFlag("help") << std::endl;
    std::cout << "print version: " << p.getFlag("version") << std::endl;

    std::cout << "Pid files: " << std::endl;
    std::vector<std::string> values = p.getAll("pid");
    for(auto i = values.begin(); i != values.end(); ++i) {
        std::cout << "   " << *i << std::endl;
    }
}
