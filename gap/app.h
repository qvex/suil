//
// Created by dc on 6/6/16.
//

#ifndef GAR_APP_H
#define GAR_APP_H

#include "middleware_pipeline.h"
#include "resource_handler.h"

namespace gap {

    class AppPriv;
    class App : public std::enable_shared_from_this<App> {
    public:
        typedef std::shared_ptr<App> Ptr;
        App();
        MiddlewarePipeline& nextMiddlewareFactory(MiddlewareFactory::Ptr factory);
        void setResourceHandler(ResourceHandler::Ptr handler);
        std::shared_ptr<AppPriv> priv() const;
    private:
        friend class AppPriv;
        std::shared_ptr<AppPriv>   priv_;
    };
}

#endif //GAR_APP_H
