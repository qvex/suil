//
// Created by dc on 6/5/16.
//

#ifndef GAR_MIDDLEWARE_FACTORY_H
#define GAR_MIDDLEWARE_FACTORY_H

#include "middleware.h"

namespace gap {

    class MiddlewareFactory {
    public:
        typedef std::shared_ptr<MiddlewareFactory> Ptr;
        virtual Middleware::Ptr createMiddleware() = 0;
    };
}

#endif //GAR_MIDDLEWARE_FACTORY_H
