//
// Created by dc on 6/6/16.
//

#include "resource_handler.h"
#include "resource_handler_priv.h"
#include "log.h"

namespace gap {

    void ResourceHandlerPriv::setHandler(ResourceHandler::Ptr handler) {
        handler_ = handler;
    }

    void ResourceHandlerPriv::handle(const HttpRequest& req, HttpResponse& resp) {
        try {
            if (handler_ != nullptr) {
                handler_->handle(req, resp);
            }
        } catch (...) {
            GAP_Dbg("%s: Uncaught exception when executing resource handler\n", __FUNCTION__);
            resp = HttpResponse(500);
        }

        // force completion of request
        if (!resp.completed()) {
            resp.end();
        }
    }
}