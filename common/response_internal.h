//
// Created by dc on 6/3/16.
//

#ifndef GAR_RESPONSE_INTERNAL_H
#define GAR_RESPONSE_INTERNAL_H

#include <functional>
namespace gar {

    struct HttpResponseHandlers {
        std::function<void()>    completeRequestHandler;
        std::function<bool()>   isAliveChecker;
    };
}
#endif //GAR_RESPONSE_INTERNAL_H
