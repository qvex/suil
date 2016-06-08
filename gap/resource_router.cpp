//
// Created by dc on 6/7/16.
//

#include "resource_router.h"
#include "util.h"
#include "log.h"

namespace gap {

    RuleCollection::RuleCollection(const std::string prefix)
        : prefix_(prefix)
    {
        rules_.clear();
    }

    RouteRule& RuleCollection::add(RouteRule&& rule) {
        if (std::find_if(rules_.begin(), rules_.end(), [&rule](RouteRule& r){
                return r.path() == rule.path();
            }) != rules_.end())
        {
            GAP_assert(false, "Adding rule that was already added is not allowed: %s", rule.path().c_str());
        }

        int index = rules_.size();
        GAP_Dbg("[this:%p] Adding route rule (%s) to collection (%s)\n", this, rule.path().c_str(), prefix_.c_str());
        rules_.push_back(std::move(rule));
        return rules_[index];
    }
    void RuleCollection::handle(const HttpRequest& req, HttpResponse& resp) {
        std::string postfix = req.url().substr(prefix_.length());

        GAP_Dbg("[this:%p] handling request [%s:%s]\n", this, prefix_.c_str(), postfix.c_str());

        for(auto& rule : rules_) {
            if (rule.handle(req, resp, postfix)) {
                GAP_Dbg("[this:%p] request (%s) handled rule (%s)\n", this, req.url().c_str(), rule.prefix().c_str());
                return;
            }
        }
        GAP_Dbg("[this:%p] not matching rule for route (%s)\n", this, postfix.c_str());
        resp.statusCode() = 404;
    }
    const std::string& RuleCollection::prefix() const {
        return prefix_;
    }

    RuleCollection::Ptr RuleCollection::create(const std::string prefix) {
        return RuleCollection::Ptr(new RuleCollection(prefix));
    }

    RouteRule& Router::route(const std::string path) {
        RouteRule rule(path);
        RuleCollection::Ptr rules = routes_.search(rule.prefix());
        if (rules == nullptr) {
            rules = RuleCollection::create(rule.prefix());
            routes_.add(rule.prefix(), rules);
            GAP_Dbg("[this:%p] created new rule collection for prefix (%s)\n", this, rules->prefix().c_str());
        }

        GAP_Dbg("[this:%p] added a new rule (%s) to collection (%s)\n", this, rule.path().c_str(), rules->prefix().c_str());
        return rules->add(std::move(rule));;
    }

    void Router::handle(const HttpRequest& req, HttpResponse& resp) {
        RuleCollection::Ptr c = routes_.lazySearch(req.url(),
        [&req, &resp](RuleCollection::Ptr r, const std::string& url, uint i)->bool {
            r->handle(req, resp);
            return true;
        });

        if (c == nullptr) {
            GAP_Dbg("[this:%p] request not handled, resource not found (%s)\n", this, req.url().c_str());
        }
    }
}