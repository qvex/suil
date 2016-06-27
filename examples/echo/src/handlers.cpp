//
// Created by dc on 6/10/16.
//

#include "handlers.h"

void EchoApp::onStart(App::Ptr app) {
    app->nextMiddlewareFactory(MiddlewareFactory::Ptr(new EchoMiddlewareFactory()));
    Router &r = gap::UseSystemRouter(app);

    // add echo route
    r.route("/api/echo/{name:string}")
            .methods(HttpMethod::GET | HttpMethod::POST)
                    ([](const HttpRequest &req, HttpResponse &res, RouteParams &p) {
                        res.statusCode() = 200;
                        res.end(req.bodyCopy());
                    });

    r.route("/api/echo/{ename:string}/{number:int}")
            .methods(HttpMethod::GET)
                    ([](const HttpRequest &req, HttpResponse &res, RouteParams &p) {
                        json::wvalue w;
                        w["ename"] = p["ename"].s();
                        w["number"] = p["number"].i();
                        res.statusCode() = 200;
                        res.end(json::dump(w));
                    });

    GAP_Route(r, "/api/log/setLevel", HttpMethod::POST)
            ([](const HttpRequest &req, HttpResponse &res, RouteParams &p) {
                const gap::json::rvalue& settings(json::load(req.bodyCopy()));
                Logger::setLogLevel((LogLevel)settings["logLevel"].i());
                res.statusCode() = 200;
                res.end();
            });

    GAP_Route(r, "/api/log/logThis", HttpMethod::PUT)
            ([](const HttpRequest &req, HttpResponse &res, RouteParams &p) {
                GAP_Info("%s\n", req.bodyCopy().c_str());
                res.statusCode() = 200;
                res.end();
            });
}
