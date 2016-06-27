//
// Created by dc on 6/4/16.
//

#ifndef GAR_SERVER_OPTIONS_H
#define GAR_SERVER_OPTIONS_H

#include <memory>
#include "json.h"

namespace gap {

    struct ServerConfig {
        uint            timeout{5};
        uint            port;
        uint            concurrency{0};
        std::string     serverName{""};
        std::string     bindAddress{"127.0.0.1"};
        static ServerConfig fromJson(const json::rvalue& jsettings);
    };
}

#endif //GAR_SERVER_OPTIONS_H
