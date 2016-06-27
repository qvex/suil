//
// Created by dc on 6/9/16.
//

#ifndef GAR_OPTIONS_H
#define GAR_OPTIONS_H

#include "server_config.h"
#include "module_config.h"
#include "app_config.h"

namespace gap {

    class Configuration {
    public:

        static Configuration loadFromFile(const std::string& configFile);
        ServerConfig&   serverConfig();
        gar::ModuleConfig& moduleConfig();
        AppConfig&  appConfig();

    private:
        ServerConfig          serverConfig_;
        gar::ModuleConfig     moduleConfig_;
        AppConfig             appConfig_;
    };
}
#endif //GAR_OPTIONS_H
