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

/**
 * @brief Parameter type
 */
typedef enum {REQUIRED, OPTIONAL} ParamType;

/**
 * @brief Parameter arguments type
 */
typedef enum {NO_ARGS, SINGLE_ARG, MULTI_ARGS} ParamArgs;

/**
 * @brief A single parameter
 */
struct Parameter {
    /**
     * @brief the long form (e.g. "file")
     */
    std::string longForm;

    /**
     * @brief the short form (e.g. "f")
     */
    std::string shortForm;

    /**
     * @brief the type of the parameter (optional, required)
     */
    ParamType type;

    /**
     * @brief the type of arguments (none, one, multiple)
     */
    ParamArgs args;

    /**
     * @brief the default value
     */
    std::string def;

    /**
     * @brief the description
     */
    std::string description;

    /**
     * @brief Default empty constructor
     */
    Parameter() { };

    /**
     * @brief Constructor with short form
     */
    Parameter(const std::string& sf, const std::string& lf,
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

    /**
     * @brief Constructor without short form
     */
    Parameter(const std::string& lf, const ParamType t = OPTIONAL,
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


/**
 * @brief Parameter parser. This class gets all necessary informations and
 * parses the parameters.
 */
class Parser {
    private:
        /**
         * @brief The values, that have already been set
         */
        std::map<std::string, std::string> values;

        /**
         * @brief All parameters, accessible via the long form
         */
        std::map<std::string, Parameter> parameters;

        /**
         * @brief All parameters, accessible via the short form
         */
        std::map<std::string, Parameter> sparameters;

        /**
         * @brief All multiple values, accessible via long form
         */
        std::map<std::string, std::vector<std::string>> multiple;

        /**
         * @brief Adds a value to the passed parameter
         */
        void addValue(const Parameter& o, const std::string& value);

    public:
        /**
         * @brief Adds the passed parameters
         */
        Parser& operator<<(const Parameter& o);

        /**
         * @brief Parses the parameters
         */
        int parse(int argc, char** argv);

        /**
         * @brief Gets a single value. If multiple values are available, it return the last added value
         *
         * @param name parameters long form
         *
         * @return value
         */
        std::string get(const std::string& name);

        /**
         * @brief Checks, if the passed flag has been set
         *
         * @param name parameters long form
         */
        bool getFlag(const std::string& name);

        /**
         * @brief Returns the value as integer
         *
         * @param name parameters long form
         *
         * @return value as integer
         */
        int getInt(const std::string& name);

        /**
         * @brief Gets all values as string
         *
         * @param name parameters long form
         *
         * @return values
         */
        std::vector<std::string> getAll(const std::string& name);

        /**
         * @brief Prints usage table
         */
        void printUsage(void);

        /**
         * @brief checks, if all required parameters are set. Throws an exception if this is not
         * the case
         */
        void checkRequired(void);
};

/**
 * @brief A parameter exception, which is thrown by Parser.
 */
class ParserException : public std::exception {
    private:
        /**
         * @brief the exception message
         */
        std::string message;

    public:
        /**
         * @brief Default constructor
         */
        ParserException(const std::string& message, const std::string& longForm);

        virtual ~ParserException(void) throw();

        /**
         * @brief returns the exception message
         *
         * @return
         */
        const char* what(void);

};

}

#endif
