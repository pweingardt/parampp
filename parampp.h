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
#ifndef OPTIONS_H
#define OPTIONS_H

#include <vector>
#include <map>
#include <string>
#include <exception>

namespace parampp {

typedef enum {REQUIRED, OPTIONAL} ParamType;
typedef enum {NO_ARGS, SINGLE_ARG, MULTI_ARGS} ParamArgs;

struct Option {
    std::string longForm;
    ParamType type;
    ParamArgs args;

    std::string shortForm;
    std::string def;

    std::string description;

    Option() { };

    Option(const std::string& sf, const std::string& lf,
            const ParamType t = OPTIONAL, const ParamArgs a = NO_ARGS,
            const std::string& desc = "",
            const std::string& def = "") {
        this->shortForm = sf;
        this->longForm = lf;
        this->type = t;
        this->args = a;
        this->def = def;
        this->description = desc;

        if(t == OPTIONAL && a == NO_ARGS && def == "") {
            this->def = "0";
        }

    }

    Option(const std::string& lf, const ParamType t = OPTIONAL,
            const ParamArgs a = NO_ARGS, const std::string& desc = "",
            const std::string& def = "") {
        this->shortForm = "";
        this->longForm = lf;
        this->type = t;
        this->args = a;
        this->def = def;
        this->description = desc;
    }
};

class Parameters {
    private:
        std::map<std::string, std::string> values;
        std::map<std::string, Option> options;
        std::map<std::string, Option> soptions;

        std::map<std::string, std::vector<std::string>> multiple;

        void addValue(const Option& o, const std::string& value);

    public:
        Parameters& operator<<(const Option& o);

        int parse(int argc, char** argv);

        std::string get(const std::string& name);
        bool getFlag(const std::string& name);
        int getInt(const std::string& name);
        std::vector<std::string> getAll(const std::string& name);

        void printUsage(void);

        void checkRequired(void);
};

class ParameterException : public std::exception {
    private:
        std::string message;
        std::string parameter;

    public:
        ParameterException(const std::string&, const std::string&);

        virtual ~ParameterException(void) throw();

        const char* what(void);

};

}

#endif
