//
// Created by dc on 6/9/16.
//
#include "json.h"
#include "configuration.h"
#include "util.h"
#include "log.h"

namespace gar {
    ModuleConfig ModuleConfig::fromJson(const gap::json::rvalue &configJson) {
        ModuleConfig   config;
        GAP_assert(configJson.has("server"), "Module configuration missing from the config file");
        const gap::json::rvalue& moduleJson = configJson["module"];

        // soname required
        GAP_assert(moduleJson.has("soname"), "The soname of the library implementing the file is required");
        config.soname = moduleJson["soname"].s();

        if (moduleJson.has("prefix")) {
            config.prefix = moduleJson["prefix"].s();
        }

        if(moduleJson.has("main")) {
            config.main = moduleJson["main"].s();
        }

        if (moduleJson.has("exit")) {
            config.exit = moduleJson["exit"].s();
        }

        GAP_Dbg("Module config: soname=%s, prefix=%s, main=%s, exit=%s\n",
                    config.soname.c_str(), config.prefix.c_str(), config.main.c_str(),
                    config.exit.c_str());

        return config;
    }
}

namespace gap {

    ServerConfig ServerConfig::fromJson(const json::rvalue &configJson) {
        GAP_assert(configJson.has("server"), "Server configuration missing from the config file");

        ServerConfig config;
        const json::rvalue &srvJson = configJson["server"];
        if (srvJson.has("serverName")) {
            config.serverName = srvJson["serverName"].s();
        }

        // port required
        GAP_assert(srvJson.has("port"), "ERROR: The configuration does not specify the listening port");
        config.port = srvJson["port"].i();

        // If the bind address is not set the default is 127.0.0.1
        if (srvJson.has("bindAddress")) {
            config.bindAddress = srvJson["bindAddress"].s();
        }

        // If concurrency is not set, default ot 0 (or maximum number of cores)
        if (srvJson.has("concurrency")) {
            config.concurrency = srvJson["concurrency"].i();
        }

        if (srvJson.has("timeout")) {
            config.timeout = srvJson["timeout"].i();
        }
        GAP_Dbg("Server config: name=%s, bind=%s, port=%d, concurrency=%d timeout=%d\n",
                config.serverName.c_str(), config.bindAddress.c_str(), config.port, config.concurrency, config.timeout);

        return config;
    }


    AppConfig AppConfig::fromJson(const json::rvalue &configJson) {
        GAP_assert(configJson.has("app"), "Application configuration missing from the config file");
        AppConfig config;
        const json::rvalue& appJson = configJson["app"];

        // The application name is required
        GAP_assert(appJson.has("name"), "ERROR: Application name missing from the configuration file");
        config.name = appJson["name"].s();

        if (appJson.has("administrator")) {
            config.administrator = appJson["administrator"].s();
        }

        if (appJson.has("ssl")) {
            if (appJson["ssl"].has("sslcert")) {
                config.sslcert = appJson["ssl"]["sslcert"].s();
            }

            if (appJson["ssl"].has("enabled")) {
                config.enableSsl = appJson["ssl"]["enabled"].b();
            }
        }

        if (appJson.has("workingDir")) {
            config.workingDir = appJson["workingDir"].s();
        }
        GAP_Dbg("App config: name=%s, admin=%s, sslcert=%s enableSsl=%dworkingDir=%s\n",
                    config.name.c_str(), config.administrator.c_str(),
                config.sslcert.c_str(), config.enableSsl, config.workingDir.c_str());
    }

    Configuration Configuration::loadFromFile(const std::string& configFile) {

        std::ifstream ifs(configFile);

        // Ensure that the .gap file exists
        GAP_assert(ifs.good(), "GAR requires the .gap file to exist");
        std::string configStr;
        ifs.seekg(0, std::ios::end);
        configStr.reserve(ifs.tellg());
        ifs.seekg(0, std::ios::beg);

        configStr.assign((std::istreambuf_iterator<char>(ifs)),
                                std::istreambuf_iterator<char>());

        gap::json::rvalue   configJson(gap::json::load(configStr));

        Configuration config;
        // load settings to settings object and validate
        config.appConfig_ = AppConfig::fromJson(configJson);
        config.serverConfig_ = ServerConfig::fromJson(configJson);
        config.moduleConfig_ = gar::ModuleConfig::fromJson(configJson);

        ifs.close();

        return  config;
    }

    ServerConfig&   Configuration::serverConfig() {
        return serverConfig_;
    }

    gar::ModuleConfig& Configuration::moduleConfig() {
        return moduleConfig_;
    }

    AppConfig&  Configuration::appConfig() {
        return appConfig_;
    }
}