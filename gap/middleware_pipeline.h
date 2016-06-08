//
// Created by dc on 6/5/16.
//

#ifndef GAR_MIDDLEWARE_PIPELINE_H
#define GAR_MIDDLEWARE_PIPELINE_H

#include "middleware_factory.h"

namespace gap {

    class MiddlewarePipelinePriv;
    class MiddlewarePipeline {
    public:
        typedef std::shared_ptr<MiddlewarePipeline> Ptr;
        MiddlewarePipeline();
        MiddlewarePipeline& nextMiddlewareFactory(MiddlewareFactory::Ptr factory);
        std::shared_ptr<MiddlewarePipelinePriv> priv() const;
    private:
        std::shared_ptr<MiddlewarePipelinePriv> priv_;
    };
}
#endif //GAR_MIDDLEWARE_PIPELINE_H
