//
// Created by dc on 6/7/16.
//

#ifndef GAR_ROUTE_PARAMS_PARSER_H
#define GAR_ROUTE_PARAMS_PARSER_H

#include <regex>
#include "route_params.h"

namespace gap {

    class RouteParamsParser {
    public:
        typedef std::vector<std::pair<std::string, ParamValue::Type>> ParamList;
        RouteParamsParser(RouteParamsParser&& other);
        RouteParamsParser&operator=(RouteParamsParser&& other);

        RouteParams build(const std::string& resource, bool& status);
        const std::string& prefix() const;

        void dump();

        static RouteParamsParser compile(const std::string& route);

    private:
        RouteParamsParser(){}
        static void process(const char& c);
        static ParamValue::Type validateType();

        /**
         * To avoid memory foot print there is only one parser state,
         * this means routes can be installed in a linear fashion, and in
         * only one thread. Each route has a parser but the path are compiled
         * only at route installation.
         */
        struct ParseContext {
            char            state_{'/'};
            std::string     name_{""};
            std::string     type_{""};
            std::string     prefix_{""};
            bool            active_{false};
            bool            prefixing_{true};
            void            acquire() {
                assert(active_==false && "ERROR: Route params parser in use");
                active_ = true;
                prefixing_ = true;
                clear();
            }

            void            release() {
                active_ = false;
            }

            void clear() {
                state_ = '/';
                name_.clear();
                type_.clear();
            }
        };

        static ParseContext         ctx_;
        static  RouteParamsParser   parser_;

    private:
        ParamList           paramDefs_;
        std::regex          regex_;
        std::string         prefix_;
    };
}
#endif //GAR_ROUTE_PARAMS_PARSER_H
