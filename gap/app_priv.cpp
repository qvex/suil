//
// Created by dc on 6/6/16.
//

#include "app_priv.h"
#include "resource_handler_priv.h"

namespace gap {
    AppPriv::AppPriv()
        : resourceHandler_(new ResourceHandlerPriv())
    {
        serverOptions_.bindAddress  = "127.0.0.1";
        serverOptions_.port         = 1080;
        serverOptions_.serverName   = "Default Server";
        serverOptions_.concurrency  = 0;
    }

    MiddlewarePipeline& AppPriv::nextMiddlewareFactory(MiddlewareFactory::Ptr factory) {
        return mwPipeline_.nextMiddlewareFactory(factory);
    }

    ServerOptions& AppPriv::serverOptions() {
        return serverOptions_;
    }

    MiddlewarePipelinePriv::Ptr AppPriv::middlewarePipeline() {
        return mwPipeline_.priv();
    }

    ResourceHandlerPriv::Ptr AppPriv::resourceHandler() {
        return resourceHandler_;
    }

    void AppPriv::setResourceHandler(ResourceHandler::Ptr handler) {
        if (resourceHandler_ != nullptr) {
            resourceHandler_->setHandler(handler);
        }
    }
}