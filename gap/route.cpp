//
// Created by dc on 6/7/16.
//

#include "route.h"

namespace gap {

    RouteRule::RouteRule(const std::string route)
        : parser_(RouteParamsParser::compile(route)),
          handler_(nullptr),
          methods_(HttpMethod::GET),
          name_(""),
          path_(route)
    {
    }

    RouteRule::RouteRule(RouteRule&& r)
        : parser_(std::move(r.parser_)),
          handler_(std::move(r.handler_)),
          methods_(r.methods_),
          name_(std::move(r.name_)),
          path_(std::move(r.path_))
    {
        r.handler_ = nullptr;
    }

    RouteRule& RouteRule::operator=(RouteRule&& r) {
        parser_ = std::move(r.parser_);
        handler_ = std::move(r.handler_);
        name_ = std::move(r.name_);
        path_ = std::move(r.path_);
        methods_ = r.methods_;
        r.handler_ = nullptr;
    }

    RouteRule& RouteRule::methods(const HttpMethod m) {
        methods_ = m;
        return *this;
    }

    RouteRule& RouteRule::name(const std::string name) {
        name_ = name;
        return *this;
    }

    RouteRule& RouteRule::operator()(RuleHandler h) {
        handler_ = h;
        return *this;
    }

    bool RouteRule::handle(const HttpRequest& req, HttpResponse& res, const std::string& postfix) {
        if (handler_ != nullptr && (req.method() & methods_) == req.method() ) {
            bool status = false;
            // this will parse the rest of the url
            RouteParams params = parser_.build(postfix, status);
            if (status) {
                handler_(req, res, params);
            }
            return status;
        }
        return false;
    }

    const std::string& RouteRule::path() const {
        return path_;
    }

    const std::string& RouteRule::prefix() const {
        return parser_.prefix();
    }
}
