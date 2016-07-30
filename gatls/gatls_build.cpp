//
// Created by dc on 6/26/16.
//

#include <string>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "gatls_cli.h"
#include "log.h"

namespace gatls {

    struct BuildOptions {
        std::string                 flavor{"debug"};
        std::vector<std::string>    defines;

        static bool         parse(BuildOptions& options, int argc, char* argv[]);
    };

    void showBuildHelp() {
    }

    void showCleanHelp() {
    }

    bool BuildOptions::parse(BuildOptions &options, int argc, char **argv) {
        int         arg = 0;
        size_t      alen = 0;
        char        cwd[1024];

        // Get the rest of the arguments
        while (arg < argc) {
            alen = strlen(argv[arg]);
            if (alen > 8 && strncmp(argv[arg], "--flavor=", 9) == 0) {
                std::string flavor = argv[arg];
                options.flavor = boost::to_lower_copy(flavor.substr(9));
            }
            else if (alen > 2 && strncmp(argv[arg], "-D", 2) == 0) {
                std::string def = argv[arg];
                options.defines.push_back(def);
            } else {
                GAP_Perr("Gatls", "unknown command line option '%s' in build command", argv[arg]);
                return false;
            }
            arg++;
        }

        options.flavor[0] = toupper(options.flavor[0]);

        // Gatls requires this defines to work correctly
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            GAP_Perr("Gatls", " Unable to pwd - %s", strerror(errno));
            return false;
        }

        if (std::find_if(options.defines.begin(), options.defines.end(), [](std::string &opt){
                return boost::starts_with(opt, "-DGAP_BINARY_PREFIX=");
        }) == options.defines.end()) {

            std::string installPrefix = cwd;
            installPrefix = "-DGAP_BINARY_PREFIX=" + installPrefix + "/bin";
            options.defines.push_back(installPrefix);
        }

        if (std::find_if(options.defines.begin(), options.defines.end(), [](std::string &opt){
            return boost::starts_with(opt, "-DGAP_DEV_BINARY_DIR=");
        }) == options.defines.end()) {
            std::string cmakeModulePath = cwd;
            cmakeModulePath = "-DGAP_DEV_BINARY_DIR=" + cmakeModulePath + "/lib;/usr/local";
            options.defines.push_back(cmakeModulePath);
        }

        return  true;
    }

#define     CMAKE_MAX_ARGS      (16)

    void executeCmake(void *arg) {
        int     argc;
        char    *argv[CMAKE_MAX_ARGS];
        char    *cmd = (char *)"cmake";
        BuildOptions *options = static_cast<BuildOptions *>(arg);

        argc = 0;
        argv[argc++] = cmd;
        argv[argc++] = (char *)"..";

        // append the build flavor if specified
        char    buildFlavor[32];
        int len = sprintf(buildFlavor, "-DCMAKE_BUILD_TYPE=%s", options->flavor.c_str());
        argv[argc++] = buildFlavor;
        argv[len] = '\0';

        // append all the other defines
        for(std::string& def: options->defines) {
            if (argc == CMAKE_MAX_ARGS) {
                GAP_Perr("Gatls", "too many defines specified, limit defines to 14");
                return;
            }
            argv[argc++] = (char *)def.c_str();;
        }

        argv[argc]  = NULL;

        // Change build directory to build
        chdir(".build");
        execvp(cmd, argv);
    }

#define MAKE_MAX_ARGS       (8)
    void executeMake(void *arg) {
        int         argc;
        char        *argv[MAKE_MAX_ARGS];
        char        *cmd = (char *)"make";

        argc = 0;
        argv[argc++] = cmd;
        argv[argc++] = (char *)"install";
        argv[argc]   = NULL;

        // Change directory to build directory and make the
        // application
        chdir(".build");
        execvp(cmd, argv);
    }

    int build(int argc, char *argv[]) {
        BuildOptions buildOptions;
        if (!BuildOptions::parse(buildOptions, argc, argv)) {
            // TODO: Show build help
            return false;
        }

        std::string cmakeLists = "CMakeLists.txt";
        // We can only build if the current directory is an application
        if (!boost::filesystem::exists(cmakeLists)) {
            GAP_Perr("Gatls", "this is not a valid application directory");
            return false;
        }

        std::string buildDir =  ".build";
        // Valid application, then check if there is already an .build directory, if not
        // create it
        if (!boost::filesystem::exists(buildDir)) {
            GAP_Put("Gatls", "build - creating build directory");
            boost::system::error_code ec;
            if (!boost::filesystem::create_directory(buildDir, ec)) {
                GAP_Perr("Gatls", "could not create build directory - %s", ec.message().c_str());
                return  false;
            }
        }


        // Configure project build with cmake
        if (!spawnCommand(executeCmake, &buildOptions)) {
            GAP_Perr("Gatls", "build - configuring project build failed");
            return false;
        }

        GAP_Pinf("Gatls", "configuring project build successful");

        // Build the project using make
        if (!spawnCommand(executeMake, NULL)) {
            GAP_Perr("Gatls", "project build failed, see output for more info");
            return false;
        }

        GAP_Pinf("Gatls", "build - successful");
        return true;
    }

    /**
     * Clean is a very simple command
     */
    int clean(int argc, char *argv[]) {
        (void) argc;
        (void) argv;

        std::string buildDir =  ".build";
        std::string binDir = "bin";
        if (boost::filesystem::exists(buildDir)) {
            GAP_Put("Gatls", "removing build history");
            boost::system::error_code ec;
            boost::filesystem::remove_all(buildDir, ec);
            if (ec) {
                GAP_Perr("Gatls", "removing build history failed - %s", ec.message().c_str());
                return  EXIT_FAILURE;
            }
        }

        if (boost::filesystem::exists(binDir)) {
            GAP_Put("Gatls", "removing binary history");
            boost::system::error_code ec;
            boost::filesystem::remove_all(binDir, ec);
            if (ec) {
                GAP_Perr("Gatls", "removing binary history failed - %s", ec.message().c_str());
                return  EXIT_FAILURE;
            }
        }

        GAP_Pinf("Gatls", "successfully cleaned application build");
        return 0;
    }

}