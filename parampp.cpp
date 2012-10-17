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

#include "parampp.h"
#include <iostream>

/**
 * @file parampp.cpp
 * @author Paul W.
 * @brief the Parser implementation
 */

namespace parampp {

Parser& Parser::operator<<(const Parameter& o) {
    if(o.longForm == "") {
        throw ParameterException("Empty long form parameter not allowed: ", o.shortForm);
    }

    if(this->parameters.find(o.longForm) == this->parameters.end() &&
            (this->sparameters.find(o.shortForm) == this->sparameters.end()
             || o.shortForm == "")) {
        this->parameters[o.longForm] = o;
        this->sparameters[o.shortForm] = o;
    } else {
        throw ParameterException("Parameter already defined: ", o.longForm);
    }

    return *this;
}

void Parser::printUsage(void) {
    unsigned int longest = 0;

    std::cout << "Parser: '-x value', '--xy=value', "
        "'-x 1st_arg 2nd_arg ...', " << std::endl << "    '--xy=1st_arg --xy=2nd_arg', "
        "or '-x 1st_arg (...) -x 2nd_arg'" << std::endl;
    std::cout << "Flags: '-h' or '--help=(0|1)'." << std::endl;

    for(auto iter = this->parameters.begin(); iter != this->parameters.end(); ++iter) {
        const Parameter& o = iter->second;
        if(longest < o.longForm.length() + o.shortForm.length()) {
            longest = o.longForm.length() + o.shortForm.length();
        }
    }

    for(auto iter = this->parameters.begin(); iter != this->parameters.end(); ++iter) {
        unsigned int width = 8;

        const Parameter& o = iter->second;
        if(o.shortForm != "") {
            std::cout << "-" << o.shortForm << ", ";
        } else {
            std::cout << "    ";
        }

        std::cout << "--" << o.longForm;
        int shortLength = (o.shortForm == "" ? 1 : o.shortForm.length());
        for(unsigned int i = 0; i < longest - shortLength - o.longForm.length() + width; ++i) {
            std::cout << " ";
        }
        std::cout << o.description;
        if(o.type == REQUIRED) {
            std::cout << " (required)";
        }
        std::cout << std::endl;
    }
}

int Parser::parse(int argc, char** argv) {
    Parameter* current = 0;
    for(int i = 1; i < argc; ++i) {
        std::string p(argv[i]);
        bool parsed = false;

        if(p.length() >= 2) {
            if(p[0] == '-' && p[1] == '-') {
                std::string param(p.substr(p.find_first_not_of("-")));
                std::string pname;

                if(param.find("=") == std::string::npos) {
                    pname = param;
                } else {
                    pname = param.substr(0, param.find_first_of("="));
                }

                if(this->parameters.find(pname) == this->parameters.end()) {
                    throw ParameterException("Unknown parameter: ", pname);
                }

                const Parameter& op = this->parameters[pname];
                std::string value;

                if(param.find("=") == std::string::npos) {
                    value = "1";
                } else {
                    value = param.substr(param.find_first_of("=") + 1);
                }

                addValue(op, value);
                parsed = true;

                current = 0;
            } else {
                if(p[0] == '-') {
                    std::string pname(p.substr(p.find_first_not_of("-")));

                    if(this->sparameters.find(pname) == this->sparameters.end()) {
                        throw ParameterException("Unknown parameter: ", pname);
                    }

                    const Parameter& o = this->sparameters[pname];

                    if(o.args == NO_ARGS) {
                        this->values[o.longForm] = "1";
                    } else {
                        current = &this->sparameters[pname];
                    }

                    parsed = true;
                }
            }
        }

        if(parsed == false) {
            if(current == 0) {
                throw ParameterException("Argument parsing error: ", p);
            }

            addValue(*current, p);

            if(current->args != MULTI_ARGS) {
                current = 0;
            }
        }
    }

    if(current != 0 && current->args != MULTI_ARGS) {
        throw ParameterException("Parameter not specified: ", current->longForm);
    }

    // add default values if necessary
    for(auto iter = this->parameters.begin(); iter != this->parameters.end(); ++iter) {
        if(iter->second.def != "") {
            if(this->values.find(iter->first) == this->values.end()) {
                addValue(iter->second, iter->second.def);
            }
        }
    }

    return 0;
}

void Parser::checkRequired() {
    // Check if all required parameters are available
    for(auto iter = this->parameters.begin(); iter != this->parameters.end(); ++iter) {
        if(iter->second.type == REQUIRED && this->values.find(iter->first) == this->values.end()) {
            throw ParameterException("Required parameter not specified: ", iter->first);
        }
    }
}

void Parser::addValue(const Parameter& o, const std::string& value) {
    if(value == "") {
        throw ParameterException("Parameter format exception: ", o.longForm);
    }

    switch(o.args) {
        case NO_ARGS:
            if(value != "1" && value != "0") {
                throw ParameterException("Parameter flags are either '0' or '1': ", o.longForm);
            }
            this->values[o.longForm] = value;
            break;

        case SINGLE_ARG:
            if(this->values.find(o.longForm) != this->values.end()) {
                throw ParameterException("Parameter already defined: ", o.longForm);
            }
            this->values[o.longForm] = value;
            break;

        case MULTI_ARGS:
            this->multiple[o.longForm].push_back(value);
            this->values[o.longForm] = value;
            break;
    }
}

std::string Parser::get(const std::string& name) {
    return this->values[name];
}

bool Parser::getFlag(const std::string& name) {
    int v = this->getInt(name);
    return (v == 0 ? false : true);
}

int Parser::getInt(const std::string& name) {
    int v = atoi(this->values[name].c_str());
    return v;
}

std::vector<std::string> Parser::getAll(const std::string& name) {
    return this->multiple[name];
}

ParameterException::ParameterException(const std::string& message,
        const std::string& parameter) {
    this->message = message;
    this->parameter = parameter;
}

ParameterException::~ParameterException(void) throw() {
}

const char* ParameterException::what(void) {
    return std::string(this->message + this->parameter).c_str();
}

}
