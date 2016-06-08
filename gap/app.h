//
// Created by dc on 6/6/16.
//

#ifndef GAR_APP_H
#define GAR_APP_H

#include "middleware_pipeline.h"
#include "resource_handler.h"

namespace gap {

    class AppPriv;
    class App {
    public:
        App();
        MiddlewarePipeline& nextMiddlewareFactory(MiddlewareFactory::Ptr factory);
        void setResourceHandler(ResourceHandler::Ptr handler);
        std::shared_ptr<AppPriv> priv() const;
    private:
        std::shared_ptr<AppPriv>   priv_;
    };
}

#endif //GAR_APP_H
