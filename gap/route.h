//
// Created by dc on 6/7/16.
//

#ifndef GAR_ROUTE_H
#define GAR_ROUTE_H

#include "resource_handler.h"
#include "route_params_parser.h"

namespace gap {

    /**
     * @brief The binding between a route and a resource on the server. The rule defines,
     * how the resource can be reached and how to parse the url before handling the request
     */
    class RouteRule {
        typedef std::function<void(const HttpRequest&, HttpResponse&, RouteParams&)> RuleHandler;
    public:
        /**
         * @brief Create a new route rule for the given route the given route
         * @param route: a string representing the route rule
         */
        RouteRule(const std::string route);
        RouteRule(RouteRule&& r);
        RouteRule&operator=(RouteRule&& r);
        RouteRule& methods(const HttpMethod m);
        RouteRule& name(const std::string name);
        RouteRule&operator()(RuleHandler h);
        bool handle(const HttpRequest& req, HttpResponse& res, const std::string& postfix);
        const std::string& path() const;
        const std::string& prefix() const;
    private:
        RouteParamsParser   parser_;
        std::string         name_;
        HttpMethod          methods_;
        RuleHandler         handler_;
        std::string         path_;
    };
}

#endif //GAR_ROUTE_H
