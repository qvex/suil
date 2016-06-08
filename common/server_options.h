//
// Created by dc on 6/4/16.
//

#ifndef GAR_SERVER_OPTIONS_H
#define GAR_SERVER_OPTIONS_H

#include <memory>

namespace gap {

    struct ServerOptions {
        typedef std::shared_ptr<gap::ServerOptions> Ptr;
        uint            port;
        uint            concurrency;
        std::string     serverName;
        std::string     bindAddress;
    };
}

#endif //GAR_SERVER_OPTIONS_H
