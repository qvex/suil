//
// Created by dc on 6/8/16.
//

#include <fstream>
#include <dlfcn.h>

#include "gar.h"
#include "app_priv.h"

namespace  gar {

    Gar::Gar(gap::Configuration config, void* handle, AppMain main, AppExit exit)
        : config_(config),
          moduleHandle_(handle),
          appMain_(main),
          appExit_(exit)
    {
    }

    Gar::Ptr Gar::loadApp(const std::string& configFile) {
        gap::Configuration config = gap::Configuration::loadFromFile(configFile);
        GAP_Dbg("Configurations successfully loaded from file (%s)\n", configFile.c_str());

        std::string filename = config.moduleConfig().prefix + "/" + config.moduleConfig().soname;
        void *handle = dlopen(&filename[0], RTLD_LAZY);
        GAP_assert(handle != nullptr, "Unable to open application binary: %s", dlerror());
        // clear errors if any

        const char* main = config.moduleConfig().main.c_str();
        AppMain mainFn = (AppMain) dlsym(handle, main);
        const  char* err = dlerror();
        GAP_assert(err == nullptr, "Unable to retrieve main (%s) function in binary: %s\n",
                        main, err);

        const char* exit = config.moduleConfig().exit.c_str();
        AppExit  exitFn =(AppExit) dlsym(handle, exit);
        err = dlerror();
        GAP_assert(err == nullptr, "Unable to retrieve main (%s) function in binary: %s\n",
                   main, err);

        GAP_Dbg("App binary(%p) successfully loaded, main=%p\n", handle, mainFn);

        return Gar::Ptr(new Gar(std::move(config), handle, mainFn, exitFn));
    }

    void Gar::load() {
        // Inilialize IoService pool
        IoServicePool::init(config_.serverConfig().concurrency);

        GAP_assert(appMain_ != nullptr, "Main is unexpectedly not available");
        appHandler_ = gap::AppHandler::Ptr(appMain_());
        GAP_assert(appHandler_ != nullptr, "Calling application supplied main returned null pointer");

        GAP_Dbg("[this:%p] application handler for (%s) successfully loaded\n", this,
                        config_.appConfig().name.c_str());

        // tell the application that its handler has successfully been loaded, giving it an app instance
        app_ = gap::App::Ptr(new gap::App());
        appHandler_->onLoaded(app_);
    }

    void Gar::start() {
        // tell the application that we are ready to start, handler
        // should initialize the application handlers such as the pipeline
        // and the resource handler
        appHandler_->onStart(app_);

        // application has done it's setup, create the server
        if (config_.appConfig().enableSsl) {
#ifdef GAR_OPENSSL_ENABLED
            server_ = Server::Ptr(
                    new SslSocketServer(
                                 config_.serverConfig(),
                                 IoServicePool::instance(),
                                 app_->priv()->middlewarePipeline(),
                                 app_->priv()->resourceHandler()));
#else
            GAP_assert(false, "Ssl server request but this version of suil was built without ssl support");
#endif
        } else {
            server_ = Server::Ptr(
                    new SocketServer(
                            config_.serverConfig(),
                            IoServicePool::instance(),
                            app_->priv()->middlewarePipeline(),
                            app_->priv()->resourceHandler()));
        }

        GAP_Dbg("Server (%p) successfully created, ready to start\n", server_.get());
        server_->run();

        // We need to send the application a notification that our server started
        appHandler_->onStarted(app_);
    }

    void Gar::stop() {
        if (server_ != nullptr) {
            // At this point we are stopping the server and we should notify the client
            // TODO: [cm-9-06-16] We might need to do more than cleaning up here
            server_->stop();

            // tell the application that the server is stopping
            appHandler_->onStop(app_);

            // Setting the server to nullptr should free it
            server_ = nullptr;
        }
    }

    void Gar::unload() {
        // before unloading module we need to ensure that the server is stopped
        stop();

        if (appHandler_ != nullptr) {
            // We also need to notify the application that the module will be unloaded
            appHandler_->onUnload(app_);

            // We set the app handler to null, it will be freed if the application does,
            // not own any references to it
            appHandler_ = nullptr;
        } else {
            GAP_Err("[this:%p] Cannot notify application, handler already freed\n", this);
        }

        if (moduleHandle_ != nullptr) {
            // We call the application's module exit function
            if (appExit_ != nullptr) {
                appExit_(0);
            }

            // free the application
            app_ = nullptr;
            dlclose(moduleHandle_);
            moduleHandle_ = nullptr;
            GAP_Info("Application module unload\n");
        } else {
            GAP_Err("[this:%p] Cannot close module handle (null)\n", this);
        }
    }

    void Gar::waitExit() {
        if (server_ != nullptr) {
            server_->wait();
        }
    }
}