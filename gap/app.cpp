//
// Created by dc on 6/6/16.
//

#include "app_priv.h"
#include "app.h"

namespace gap {

    App::App() {
        priv_ = AppPriv::Ptr(new AppPriv());
    }

    MiddlewarePipeline& App::nextMiddlewareFactory(MiddlewareFactory::Ptr factory) {
        return priv_->nextMiddlewareFactory(factory);
    }
    void App::setResourceHandler(ResourceHandler::Ptr handler) {
        priv_->setResourceHandler(handler);
    }

    AppPriv::Ptr App::priv() const {
        return priv_;
    }

}