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

namespace parampp {

Parameters& Parameters::operator<<(const Option& o) {
    if(o.longForm == "") {
        throw ParameterException("Empty long form parameter not allowed: ", o.shortForm);
    }

    if(this->options.find(o.longForm) == this->options.end() &&
            (this->soptions.find(o.shortForm) == this->soptions.end()
             || o.shortForm == "")) {
        this->options[o.longForm] = o;
        this->soptions[o.shortForm] = o;
        if(o.type == OPTIONAL && o.args == NO_ARGS) {
                this->values[o.longForm] = "0";
        }

        if(o.def != "") {
            this->values[o.longForm] = o.def;
        }
    } else {
        throw ParameterException("Option already defined: ", o.longForm);
    }

    return *this;
}

void Parameters::printUsage(void) {
    unsigned int longest = 0;

    std::cout << "Parameter format is either '-x value' or '--xy=value'" << std::endl;

    for(auto iter = this->options.begin(); iter != this->options.end(); ++iter) {
        const Option& o = iter->second;
        if(longest < o.longForm.length() + o.shortForm.length()) {
            longest = o.longForm.length() + o.shortForm.length();
        }
    }

    for(auto iter = this->options.begin(); iter != this->options.end(); ++iter) {
        unsigned int width = 8;

        const Option& o = iter->second;
        if(o.shortForm != "") {
            std::cout << "-" << o.shortForm << ", ";
        } else {
            std::cout << "   ";
        }

        std::cout << "--" << o.longForm;
        for(unsigned int i = 0; i < longest - o.shortForm.length() - o.longForm.length() + width; ++i) {
            std::cout << " ";
        }
        std::cout << o.description;
        if(o.type == REQUIRED) {
            std::cout << "   (required)";
        }
        std::cout << std::endl;
    }
}

int Parameters::parse(int argc, char** argv) {
    Option* current = 0;
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

                if(this->options.find(pname) == this->options.end()) {
                    throw ParameterException("Unknown parameter: ", pname);
                }

                const Option& op = this->options[pname];

                if(op.args == NO_ARGS) {
                    this->values[pname] = "1";
                } else {
                    if(param.find("=") == std::string::npos) {
                        throw ParameterException("Unknown parameter format "
                                "(format should be --param=value): ", param);
                    }

                    std::string value(param.substr(param.find_first_of("=") + 1));
                    if(value == "") {
                        throw ParameterException("Unknown parameter format "
                                "(format should be --param=value): ", param);
                    }
                    this->values[pname] = value;
                }
                parsed = true;

                current = 0;
            } else {
                if(p[0] == '-') {
                    std::string pname(p.substr(p.find_first_not_of("-")));

                    if(this->soptions.find(pname) == this->soptions.end()) {
                        throw ParameterException("Unknown parameter: ", pname);
                    }

                    const Option& o = this->soptions[pname];

                    if(o.args == NO_ARGS) {
                        this->values[o.longForm] = "1";
                    } else {
                        current = &this->soptions[pname];
                    }

                    parsed = true;
                }
            }
        }

        if(parsed == false) {
            if(current == 0) {
                throw ParameterException("Argument parsing error: ", p);
            }
            this->values[current->longForm] = p;
            current = 0;
        }
    }

    if(current != 0) {
        throw ParameterException("Parameter not specified: ", current->longForm);
    }

    for(auto iter = this->options.begin(); iter != this->options.end(); ++iter) {
        if(iter->second.type == REQUIRED && this->values.find(iter->first) == this->values.end()) {
            throw ParameterException("Required parameter not specified: ", iter->first);
        }
    }

    return 0;
}

std::string Parameters::get(const std::string& name) {
    return this->values[name];
}

bool Parameters::getFlag(const std::string& name) {
    int v = this->getInt(name);
    return (v == 0 ? false : true);
}

int Parameters::getInt(const std::string& name) {
    int v = atoi(this->values[name].c_str());
    return v;
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
