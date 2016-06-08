//
// Created by dc on 6/6/16.
//

#ifndef GAR_RESOURCE_HANDLER_PRIV_H
#define GAR_RESOURCE_HANDLER_PRIV_H

#include "request.h"
#include "response.h"

namespace gap {
    class ResourceHandler;
    class ResourceHandlerPriv {
    public:
        typedef std::shared_ptr<ResourceHandlerPriv> Ptr;
        void setHandler(std::shared_ptr<ResourceHandler> handler);
        void handle(const HttpRequest& req, HttpResponse& resp);

    private:
        std::shared_ptr<ResourceHandler>        handler_;
    };
}
#endif //GAR_RESOURCE_HANDLER_PRIV_H
