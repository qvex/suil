//
// Created by dc on 6/8/16.
//

#ifndef GAR_APP_OPTIONS_H
#define GAR_APP_OPTIONS_H

#include "json.h"

namespace gap {

    struct AppConfig {
        bool                enableSsl{false};
        std::string         name{""};
        std::string         administrator{""};
        std::string         sslcert{""};
        std::string         workingDir{""};
        static AppConfig fromJson(const json::rvalue& jsettings);
    };
}
#endif //GAR_APP_OPTIONS_H
