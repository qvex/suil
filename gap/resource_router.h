//
// Created by dc on 6/7/16.
//

#ifndef GAR_RESOURCE_ROUTER_H
#define GAR_RESOURCE_ROUTER_H


#include "route.h"
#include "trie.h"
#include "resource_handler.h"

namespace gap {

    class RuleCollection {
    public:
        typedef std::shared_ptr<RuleCollection> Ptr;
        RuleCollection(const std::string prefix);
        RouteRule& add(RouteRule&& rule);
        void handle(const HttpRequest& req, HttpResponse& resp);
        const std::string& prefix() const;
        static RuleCollection::Ptr create(const std::string prefix);

    private:
        std::vector<RouteRule>   rules_;
        std::string     prefix_;
    };

    class Router : public ResourceHandler  {
    public:
        RouteRule& route(const std::string path);
        virtual void handle(const HttpRequest& req, HttpResponse& resp);

    private:
        Trie<RuleCollection>     routes_;
    };
}
#endif //GAR_RESOURCE_ROUTER_H
