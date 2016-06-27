//
// Created by dc on 6/10/16.
//

#ifndef GAR_ECHO_HANDLER_H_H
#define GAR_ECHO_HANDLER_H_H

#include "gap.h"
#include <chrono>

using namespace gap;

class EchoApp : public AppHandler {
public:
    virtual void onStart(App::Ptr app);
};

class EchoMiddleware: public Middleware {
public:
    virtual void beforeHandle(Environment& env, const HttpRequest& req, HttpResponse& resp) {
        seen_ = std::chrono::steady_clock::now();
    }
    virtual void afterHandle(Environment& env, const HttpRequest& req, HttpResponse& resp) {
        auto diff = std::chrono::steady_clock::now() - seen_;
        GAP_Dbg("Request duration: %ld us\n",
                 std::chrono::duration_cast<std::chrono::microseconds>(diff).count());
    }

private:
    std::chrono::steady_clock::time_point     seen_{};
};

class EchoMiddlewareFactory: public MiddlewareFactory {
public:
    virtual Middleware::Ptr createMiddleware() {
        return Middleware::Ptr(new EchoMiddleware());
    }
};

#endif //GAR_ECHO_HANDLER_H_H
