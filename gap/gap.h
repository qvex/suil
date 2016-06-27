//
// Created by dc on 6/8/16.
//

#ifndef GAR_GAP_H
#define GAR_GAP_H

#include "log.h"
#include "resource_router.h"
#include "app.h"
#include "app_handler.h"


namespace gap {

    inline Router& UseSystemRouter(App::Ptr app) {
        Router *r = new Router();
        app->setResourceHandler(ResourceHandler::Ptr(r));
        return *r;
    }

#define GAP_Route(router, path, method) router.route(path).methods(method)
}

#endif //GAR_GAP_H
