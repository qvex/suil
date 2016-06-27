//
// Created by dc on 6/13/16.
//

#ifndef GAR_CREATE_APP_H
#define GAR_CREATE_APP_H

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "gatls_cli.h"
#include "json.h"
#include "log.h"

namespace gatls {

    struct CreateAppOptions {
        std::string appName;
        std::string project;
        bool useSysRouter;
        bool useSsl;
        static bool parse(CreateAppOptions& options, int argc, char *argv[]);
    };

    static bool createCMakeLists(const CreateAppOptions& createAppOptions) {
        std::string filename = createAppOptions.project + "/CMakeLists.txt";
        std::ofstream cmakeLists(filename);
        if (!cmakeLists.is_open()) {
            GAP_Perr("Gatls", "creating build configuration file failed");
            return false;
        }

        cmakeLists << "CMAKE_MINIMUM_REQUIRED (VERSION 3.0)\n";
        cmakeLists << "project(" << createAppOptions.project << ")\n\n";

        cmakeLists << "set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${GAP_DEV_BINARY_DIR}/cmake)\n";
        cmakeLists << "include(GapSetup)\n\n";

        cmakeLists << "set(" << createAppOptions.project << "_ADDITIONAL_SOURCES \"\")\n";
        cmakeLists << "Gap_InitializeTarget(" << createAppOptions.project << ")\n\n";

        cmakeLists << "set(" << createAppOptions.project << "_ADDITIONAL_LIBRARIES \"\")\n";
        cmakeLists << "Gap_LoadDependecies(" << createAppOptions.project << ")\n\n";

        cmakeLists << "set(" << createAppOptions.project << "_ADDITIONAL_DEFINES \"\")\n";
        cmakeLists << "Gap_AddTarget(" << createAppOptions.project << ") \n\n";

        cmakeLists << "Gap_FinalizeTarget(" << createAppOptions.project << ")\n";

        cmakeLists.close();

        return true;
    }

    static bool createAppSourceFiles(const CreateAppOptions& createAppOptions) {
        std::string proj = createAppOptions.project;
        std::string headerFile = proj + "/src/" + proj + ".h";
        std::string sourceFile = proj + "/src/" + proj + ".cpp";

        std::string codePrefix = createAppOptions.project;
        codePrefix[0] = toupper(codePrefix[0]);

        // Create header file
        {
            std::ofstream header(headerFile);
            if (!header.is_open()) {
                GAP_Perr("Gatls", "creating application header file failed");
                return false;
            }

            std::string ucodePrefix = boost::to_upper_copy(codePrefix);
            // Open header file guards
            header << "#ifndef __" << ucodePrefix << "_H__\n";
            header << "#define __" << ucodePrefix << "_H__\n\n";

            // Includes
            header << "#include <gap/gap.h>\n\n";

            // Application handler
            header << "class " << codePrefix << "Handler: public gap::AppHandler {\n";
            header << "public:\n";
            header << "    virtual void onStart(gap::App::Ptr app);\n";
            header << "};\n\n";
            // Close header file guards
            header << "#endif //__" << ucodePrefix << "_H__";

            header.close();
        }

        // Create source file with module functions
        {
            std::ofstream source(sourceFile);
            if (!source.is_open()) {
                GAP_Perr("Gatls", "creating application source file failed");
                return false;
            }

            // Include the just created header file
            source << "#include \"" << proj << ".h\"\n\n";

            // Implement module create and exit functions
            source << "extern \"C\" {\n\n";

            source << "    void* " << codePrefix << "_Create(void) {\n";
            source << "        return new " << codePrefix << "Handler();\n";
            source << "     }\n\n";

            source << "    void " << codePrefix << "_Exit(const int32_t exitCode) { \n";
            source << "        GAP_Dbg(\"" << codePrefix << " exiting: %d \\n\", exitCode);\n";
            source << "    }\n\n";

            source << "} // extern C\n\n";
            source.close();
        }

        // Create source file implementing a simple app handler
        {
            std::string handlerSrcFile = proj + "/src/" + proj +"_handler.cpp";
            std::ofstream handler(handlerSrcFile);
            if (!handler.is_open()) {
                GAP_Perr("Gatls", "creating application handler source file failed");
                return false;
            }

            handler << "\n\n";
            handler << "#include \"" << proj << ".h\"\n\n";

            // Implement the on start method
            handler << "void " << codePrefix << "Handler::onStart(gap::App::Ptr app) {\n";
            handler << "    gap::Router& r = gap::UseSystemRouter(app);\n\n";

            handler << "    GAP_Route(r, \"/api/hello\", gap::HttpMethod::GET)\n";
            handler << "    ([](const gap::HttpRequest &req, gap::HttpResponse &res, gap::RouteParams &p) { \n";
            handler << "        res.end(200, \"Hello! World from " << codePrefix << ".\");\n";
            handler << "    });\n";
            handler << "}\n";

            handler.close();
        }

        return true;
    }

    static bool createAppConfigFile(const CreateAppOptions& createAppOptions)
    {
        std::string codePrefix = createAppOptions.project;
        codePrefix[0] = toupper(codePrefix[0]);

        {
            std::string configFile = createAppOptions.project + "/" + createAppOptions.project + ".json";
            std::ofstream appCfg(configFile);
            if (!appCfg.is_open()) {
                GAP_Perr("Gatls", "creating application handler source file failed");
                return false;
            }

            appCfg <<   "{\n";
            appCfg <<   "   \"app\": {\n";
            appCfg <<   "       \"name\": \"" << createAppOptions.appName << "\",\n";
            appCfg <<   "       \"ssl\": {\n";
            appCfg <<   "           \"enable\": " << (createAppOptions.useSsl? "true": "false") << "\n";
            appCfg <<   "       }\n";
            appCfg <<   "    },\n";
            appCfg <<   "   \"executable\": {\n";
            appCfg <<   "       \"file\": \"" << createAppOptions.project << ".gap\",\n";
            appCfg <<   "       \"create\": \"" << codePrefix << "_Create\",\n";
            appCfg <<   "       \"exit\": \"" << codePrefix << "_Exit\",\n";
            appCfg <<   "       \"prefix\": \"./\"\n";
            appCfg <<   "   },\n";
            appCfg <<   "   \"server\": {\n";
            appCfg <<   "       \"port\": " << 1080 << ",\n";
            appCfg <<   "       \"timeout\": " << 5 << "\n";
            appCfg <<   "   }\n";
            appCfg <<   "}\n";
            appCfg.close();
        }
        return true;
    }

    bool CreateAppOptions::parse(CreateAppOptions& options, int argc, char *argv[]) {
        int arg = 0;
        if (argc > 0) {
            std::string proj = argv[arg++];
            options.project = proj;

            GAP_Pinf("Gatls", "creating %s", options.project.c_str());
            while (arg < argc) {
                if (strcmp(argv[arg], "--no-ssl") == 0) {
                    options.useSsl = false;
                } else if (strcmp(argv[arg], "--no-router") == 0) {
                    options.useSysRouter = false;
                } else if (strlen(argv[arg]) > 6 && strncmp(argv[arg], "--name=", 7) == 0) {
                    std::string name = argv[arg];
                    options.appName = name.substr(7);
                } else {
                    GAP_Perr("Gatls", "unknown option %s in create command", argv[arg]);
                    return false;
                }
                arg++;
            }
        } else {
            GAP_Perr("Gatls", "create expects the application project name, see help for usage");
            return false;
        }

        return true;
    }

    int create(int argc, char *argv[]) {
        CreateAppOptions createAppOptions;

        if (!CreateAppOptions::parse(createAppOptions, argc, argv)) {
            // TODO: Show help
            return false;
        }

        // Command line options successfully parsed
        const std::string proj = createAppOptions.project;
        std::string appDir = "./"+ proj;

        // create directory if doesn't exist
        if (boost::filesystem::exists(appDir)) {
            GAP_Perr("Gatls", "application project with name %s already exist in current directory",
                   proj.c_str());
            return false;
        }

        boost::system::error_code ec;
        // try creating the directory
        if (!boost::filesystem::create_directory(appDir, ec)) {
            GAP_Perr("Gatls", "creating app directory failed- %s", ec.message().c_str());
            return false;
        }

        // create subdirectories
        std::string sourceDir = proj + "/src";
        ec.clear();
        if (!boost::filesystem::create_directory(sourceDir, ec)) {
            GAP_Perr("Gatls", "creating sources directory failed- %s", ec.message().c_str());
            return false;
        }

        std::string resourceDir = proj + "/res";
        ec.clear();
        if (!boost::filesystem::create_directory(resourceDir, ec)) {
            GAP_Perr("Gatls", "creating resource directory failed - %s", ec.message().c_str());
            return false;
        }

        GAP_Pinf("Gatls", "created app folders ./%s/{src,res}", proj.c_str());

        // create build configuration
        if (!createCMakeLists(createAppOptions)) {
            return false;
        }
        GAP_Pinf("Gatls", "created cmake build file. ./%s/CMakeLists.txt", proj.c_str());

        if (!createAppSourceFiles(createAppOptions)) {
            return false;
        }
        GAP_Pinf("Gatls", "created application source files. ./%s/src/{%s.h, %s.cpp, %s_handler.cpp}",
               proj.c_str(), proj.c_str(), proj.c_str(), proj.c_str());

        if (!createAppConfigFile(createAppOptions)) {
            return false;
        }
        GAP_Pinf("Gatls", "create application config file. ./%s/%s.json", proj.c_str(), proj.c_str());

        return true;
    }
}
#endif //GAR_CREATE_APP_H
