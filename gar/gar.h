//
// Created by dc on 6/8/16.
//

#ifndef GAR_GAR_H
#define GAR_GAR_H

#include "server.h"
#include "app_handler.h"
#include "configuration.h"

namespace  gar {

    /**
     * @brief GAR is the hook between the application and the server.
     * It loads the user application and runs in on the server using
     * setting loaded from the app.gap file.
     */
    class Gar {
        typedef gap::AppHandler* (*AppMain)(void);
        typedef void (*AppExit)(int);

    public:
        typedef std::shared_ptr<Gar>    Ptr;
        void load();
        void start();
        void stop();
        void unload();

        static Gar::Ptr loadApp(const std::string& configFile);
    private:
        Gar(gap::Configuration config, void* handle, AppMain main, AppExit exit);
        Server::Ptr             server_;
        gap::App::Ptr           app_;
        gap::AppHandler::Ptr    appHandler_;
        gap::Configuration      config_;
        void                    *moduleHandle_;
        AppMain                 appMain_;
        AppExit                 appExit_;
    };
}
#endif //GAR_GAR_H
