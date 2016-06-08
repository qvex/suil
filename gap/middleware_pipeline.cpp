//
// Created by dc on 6/6/16.
//

#include "middleware_pipeline.h"
#include "middleware_pipeline_priv.h"
namespace gap {

    MiddlewarePipeline::MiddlewarePipeline() {
        priv_ = MiddlewarePipelinePriv::Ptr(new MiddlewarePipelinePriv());
    }

    MiddlewarePipeline& MiddlewarePipeline::nextMiddlewareFactory(MiddlewareFactory::Ptr factory) {
        priv_->nextMiddlewareFactor(factory);
        return *this;
    }

    MiddlewarePipelinePriv::Ptr MiddlewarePipeline::priv() const {
        return priv_;
    }
}