//
// Created by dc on 6/5/16.
//

#ifndef GAR_MIDDLEWARE_H
#define GAR_MIDDLEWARE_H

#include "response.h"

namespace gap {

    /*
     * @brief The base class for a middleware that can be installed into the Middleware
     * pipeline
     */
    class Middleware {
    public:
        typedef std::shared_ptr<Middleware> Ptr;
        virtual void beforeHandle(Environment& env, const HttpRequest& req, HttpResponse& resp)
        {}
        virtual void afterHandle(Environment& env, const HttpRequest& req, HttpResponse& resp)
        {}
    };
}
#endif //GAR_MIDDLEWARE_H
