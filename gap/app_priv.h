//
// Created by dc on 6/6/16.
//

#ifndef GAR_APP_PRIV_H
#define GAR_APP_PRIV_H

#include "server_config.h"
#include "middleware_pipeline.h"
#include "middleware_pipeline_priv.h"
#include "resource_handler.h"
#include "resource_handler_priv.h"

namespace gap {

    class AppPriv {
    public:
        typedef std::shared_ptr<AppPriv> Ptr;
        AppPriv();
        MiddlewarePipeline& nextMiddlewareFactory(MiddlewareFactory::Ptr factory);
        MiddlewarePipelinePriv::Ptr middlewarePipeline();
        ResourceHandlerPriv::Ptr resourceHandler();
        void setResourceHandler(ResourceHandler::Ptr handler);
    private:
        MiddlewarePipeline        mwPipeline_;
        ResourceHandlerPriv::Ptr  resourceHandler_;
    };
}

#endif //GAR_APP_PRIV_H
