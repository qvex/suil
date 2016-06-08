//
// Created by dc on 6/6/16.
//

#ifndef GAR_RESOURCE_HANDLER_H
#define GAR_RESOURCE_HANDLER_H

#include "request.h"
#include "response.h"

namespace gap {

    class ResourceHandler {
    public:
        typedef std::shared_ptr<ResourceHandler> Ptr;
        virtual void handle(const HttpRequest& req, HttpResponse& resp) = 0;
    };
}

#endif //GAR_RESOURCE_HANDLER_H
