//
// Created by dc on 6/9/16.
//

#ifndef GAR_APPHANDLER_H
#define GAR_APPHANDLER_H

#include "app.h"

namespace gap {

    class AppHandler {
    public:
        typedef std::shared_ptr<AppHandler> Ptr;
        /**
         * @brief The first callback onto the application. This is invoked
         * when the application has been loaded and this handler has been
         * initialized
         */
        virtual void onLoaded(App::Ptr app) = 0;

        /**
         * @brief Called before the application is gracefully unloaded. Will not
         * the called in case of exceptions or unexpected unloads. This gives the application
         * a chance to cleanup
         */
        virtual void onUnload(App::Ptr app) = 0;

        /**
         * @brief Callback into the application before starting
         * the server to notify the client that all is good
         * and we will be listening to incoming connections
         */

        virtual void onStart(App::Ptr app) = 0;

        /**
         * @brief Callback into the application after the server
         * has been successfully started and is accepting incoming
         * connections. The application can now start scheduling work
         * into the execution pool
         */
        virtual void onStarted(App::Ptr app) = 0;

        /**
         * @brief Called before stopping the application. This tells the application
         * that it will be stopped soon, giving it a chance to clean up after itself.
         */
        virtual void onStop(App::Ptr app) = 0;

        /**
         * @brief Invoked whenever an unhandled exception that maybe from the
         * application is encountered, giving the application a chance to handle
         * the exception.
         */
        virtual bool onException(App::Ptr app) = 0;
    };
}
#endif //GAR_APPHANDLER_H
