/* Copyright (C)
 * 2012 - Paul Weingardt
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <iostream>
#include "parampp.h"

/**
 * @file example.cpp
 * @author Paul W.
 * @brief Small example
 */

/**
 * @brief Main entry function
 */
int main(int argc, char** argv) {
    parampp::Parser p;
    try {
        p << parampp::Parameter("h", "help", parampp::OPTIONAL, parampp::NO_ARGS, "Show this help")

            << parampp::Parameter("f", "file", parampp::REQUIRED, parampp::MULTI_ARGS, "the input files",
                    "default.conf")

            << parampp::Parameter("o", "out", parampp::REQUIRED, parampp::SINGLE_ARG, "the output file");
        p.parse(argc, argv);
    } catch(parampp::ParserException e) {
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
    } catch(parampp::ParserException e) {
        std::cout << e.what() << std::endl;
        p.printUsage();
        return 1;
    }

    std::cout << "Outputfile: " << p.get("out") << std::endl;
    std::cout << "Inputfiles: " << std::endl;
    std::vector<std::string> values = p.getAll("file");
    for(auto i = values.begin(); i != values.end(); ++i) {
        std::cout << "   " << *i << std::endl;
    }
    std::cout << "Print help: " << p.getFlag("help") << std::endl;

}
