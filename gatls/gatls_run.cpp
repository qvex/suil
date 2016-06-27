//
// Created by dc on 6/26/16.
//

#include <cstring>
#include <boost/filesystem.hpp>

#include "gar.h"
#include "gatls_cli.h"

namespace gatls {
    struct RunOptions {
        std::string     config{""};
        std::string     wdir{""};
        gap::LogLevel   logLevel{gap::LogLevel::INFO};
        static bool parse(RunOptions &opts, int argc, char *argv[]);
    };

    bool RunOptions::parse(RunOptions &opts, int argc, char **argv) {
        int arg = 0;
        int alen = 0;

        while (arg < argc) {
            alen = strlen(argv[arg]);
            if (alen > 6 && strncmp(argv[arg], "--conf=", 7) == 0) {
                std::string configOpt = argv[arg];
                opts.config = configOpt.substr(7);
            } else if (alen > 6 && strncmp(argv[arg], "--wdir=", 7) == 0) {
                std::string wdirOpt = argv[arg];
                opts.wdir = wdirOpt.substr(7);
            } else if (alen > 8 && strncmp(argv[arg], "--loglvl=", 9) == 0) {
                std::string loglvlStr = argv[arg];
                loglvlStr = loglvlStr.substr(9);
                int logLevel = atoi(loglvlStr.c_str());
                opts.logLevel = (gap::LogLevel) logLevel;
            }
            arg++;
        }

        // if the working directory is empty assume we are running our application from bin
        if (opts.wdir.empty()) {
            opts.wdir = "bin";
        }
        if (!boost::filesystem::exists(opts.wdir)) {
            GAP_Perr("Gatls", " '%s' working directory does not exist", opts.wdir.c_str());
            return false;
        }

        // if the config file is not specified, used directory name
        if (opts.config.empty()) {
            opts.config = boost::filesystem::current_path().filename().string() + ".json";
        }

        if (!boost::filesystem::exists(opts.config)) {
            std::string tmpConfig = opts.wdir + "/" + opts.config;
            if (boost::filesystem::exists(tmpConfig)) {
                GAP_Perr("Gatls", " '%s' config file does not exist", tmpConfig.c_str());
                return false;
            }
        }

        return true;
    }
    void runGar(void *arg) {
        RunOptions *options = static_cast<RunOptions *>(arg);
        try {
            pid_t  pid = getpid();
            // Change working directory
            chdir(options->wdir.c_str());
            GAP_Put("Gatls", "Running application, wdir=%s, pid = %d",
                    boost::filesystem::current_path().c_str(), pid);

            gar::Gar::Ptr appHost = gar::Gar::loadApp(options->config);
            gap::Logger::setLogLevel(options->logLevel);

            GAP_Put("Gatls", "loading application");
            appHost->load();
            GAP_Put("Gatls",  "starting application");

            appHost->start();
            // Wait for application to exit;
            GAP_Put("Gatls", "application started");
            appHost->waitExit();

            GAP_Put("Gatls", "goodbye\n");

            exit(EXIT_SUCCESS);
        } catch(std::exception& ex) {
            GAP_Perr("Gatls", "Error encountered while running application - %s", ex.what());
            exit(EXIT_FAILURE);
        }
    }

    int run(int argc, char *argv[]) {
        RunOptions options;
        if (!RunOptions::parse(options, argc, argv)) {
            GAP_Perr("Gatls", "Cannot run application with specified arguments");
            return EXIT_FAILURE;
        }

        gap::Logger::setLogLevel(options.logLevel);
        // run the application
        spawnCommand(runGar, &options);
        return EXIT_SUCCESS;
    }
}