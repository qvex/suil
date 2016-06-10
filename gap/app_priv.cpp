//
// Created by dc on 6/6/16.
//

#include "app_priv.h"

namespace gap {
    AppPriv::AppPriv()
        : resourceHandler_(new ResourceHandlerPriv())
    {
    }

    MiddlewarePipeline& AppPriv::nextMiddlewareFactory(MiddlewareFactory::Ptr factory) {
        return mwPipeline_.nextMiddlewareFactory(factory);
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