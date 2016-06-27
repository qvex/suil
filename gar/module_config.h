//
// Created by dc on 6/8/16.
//

#ifndef GAR_MODULE_OPTIONS_H
#define GAR_MODULE_OPTIONS_H

#include <string>

namespace gar {

    struct ModuleConfig {
        std::string     soname{""};
        std::string     main{""};
        std::string     exit{""};
        std::string     prefix{""};
        static ModuleConfig fromJson(const gap::json::rvalue& configJson);
    };
}
#endif //GAR_MODULE_OPTIONS_H
