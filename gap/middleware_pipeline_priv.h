//
// Created by dc on 6/5/16.
//

#ifndef GAR_MIDDLEWARE_PIPELINE_PRIV_H
#define GAR_MIDDLEWARE_PIPELINE_PRIV_H

#include "middleware_factory.h"
#include <deque>

namespace gap {

    class MiddlewarePipelinePriv {
    public:
        typedef std::deque<Middleware::Ptr> CallStack;
        typedef std::shared_ptr<MiddlewarePipelinePriv> Ptr;
        void nextMiddlewareFactor(MiddlewareFactory::Ptr factory);
        bool callBeforeHandlers(CallStack& cs, Environment& env, const HttpRequest& req, HttpResponse& resp);
        void callAfterHanders(CallStack& cs, Environment& env, const HttpRequest& req, HttpResponse& resp);
    private:
        std::vector<MiddlewareFactory::Ptr>     factories_;
    };
}
#endif //GAR_MIDDLEWARE_PIPELINE_PRIV_H
