//
// Created by dc on 6/6/16.
//

#include "middleware_pipeline_priv.h"

namespace gap {

    void MiddlewarePipelinePriv::nextMiddlewareFactor(MiddlewareFactory::Ptr factory) {
        factories_.push_back(factory);
    }

    bool MiddlewarePipelinePriv::callBeforeHandlers(CallStack& cs,
                                                    Environment& env,
                                                    const HttpRequest& req,
                                                    HttpResponse& resp)
    {
        bool clearCallStack{false};
        Middleware::Ptr mw = nullptr;
        for(auto& factory: factories_) {
            mw = factory->createMiddleware();
            mw->beforeHandle(env, req, resp);
            cs.push_front(mw);
            if (resp.completed()) {
                clearCallStack = true;
                break;
            }
        }

        if (clearCallStack) {
            callAfterHanders(cs, env, req, resp);
        }
    }

    void MiddlewarePipelinePriv::callAfterHanders(CallStack& cs,
                                                  Environment& env,
                                                  const HttpRequest& req,
                                                  HttpResponse& resp)
    {
        Middleware::Ptr mw = nullptr;
        while (!cs.empty()) {
            mw = cs.front();
            mw->afterHandle(env, req, resp);
            cs.pop_front();
        }
    }

}
