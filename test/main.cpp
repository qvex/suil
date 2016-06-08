//
// Created by dc on 6/1/16.
//

#include "server.h"
#include "app_priv.h"
#include "app.h"
#include "resource_router.h"

void print(uint n)
{
    if (n == 20) {
        sleep(5);
        gar::IoServicePool::instance()->schedule(boost::bind(print, 0));
    }

    GAP_Info("Executing posted work %d\n", n);
}

class EchoMiddleware : public gap::Middleware {
public:
    EchoMiddleware() {
        creations_++;
    }

    virtual void beforeHandle(gap::Environment& env, const gap::HttpRequest& req, gap::HttpResponse& resp)
    {
        GAP_Dbg("EchoMiddleware %s: creations=%d\n", __FUNCTION__, creations_);
    }
    virtual void afterHandle(gap::Environment& env, const gap::HttpRequest& req, gap::HttpResponse& resp)
    {
        GAP_Dbg("EchoMiddleware %s: creations=%d\n", __FUNCTION__, creations_);
    }

    ~EchoMiddleware() {
        GAP_Dbg("EchoMiddleware destroyed: %p\n",this);
    }

private:
    static int creations_;
};

int EchoMiddleware::creations_ = 0;

class EchoMiddlewareFactory : public gap::MiddlewareFactory {
    virtual gap::Middleware::Ptr createMiddleware() {
        return gap::Middleware::Ptr(new EchoMiddleware());
    }
};

class EchoMiddleware2 : public gap::Middleware {
public:
    EchoMiddleware2() {
        creations_++;
    }

    virtual void beforeHandle(gap::Environment& env, const gap::HttpRequest& req, gap::HttpResponse& resp)
    {
        GAP_Dbg("EchoMiddleware2 %s: creations=%d\n", __FUNCTION__, creations_);
    }
    virtual void afterHandle(gap::Environment& env, const gap::HttpRequest& req, gap::HttpResponse& resp)
    {
        GAP_Dbg("EchoMiddleware2 %s: creations=%d\n", __FUNCTION__, creations_);
    }

    ~EchoMiddleware2() {
        GAP_Dbg("EchoMiddleware2 destroyed: %p\n",this);
    }

private:
    static int creations_;
};

int EchoMiddleware2::creations_ = 0;

class EchoMiddleware2Factory : public gap::MiddlewareFactory {
    virtual gap::Middleware::Ptr createMiddleware() {
        return gap::Middleware::Ptr(new EchoMiddleware2());
    }
};

int main(int argc, char* argv[]) {
    gap::ServerOptions::Ptr opts(new gap::ServerOptions);
    gap::App app;
    app.nextMiddlewareFactory(gap::MiddlewareFactory::Ptr(new EchoMiddlewareFactory()))
            .nextMiddlewareFactory(gap::MiddlewareFactory::Ptr(new EchoMiddleware2Factory()));

    gap::Router router;
    router.route("/api/login/{uname:string}/{passwd:string}")
            .method(gap::HttpMethod::GET)
            .method(gap::HttpMethod::POST)
                    ([&](const gap::HttpRequest&, gap::HttpResponse& res, gap::RouteParams& p) {
                        gap::json::wvalue e;
                        e["name"] = p["uname"].s();
                        e["passwd"] = p["passwd"].s();
                        res.end(200, e);
                    });
    router.route("/api/login/{uname:string}")
            .method(gap::HttpMethod::GET)
                    ([&](const gap::HttpRequest&, gap::HttpResponse& res, gap::RouteParams& p) {
                        std::ostringstream os;
                        os << "Welcome user: " << p["uname"].s() << std::endl;
                        res.body() = os.str();
                        res.statusCode() = 200;
                    });

    gap::ResourceHandler::Ptr rhandler(&router);
    app.setResourceHandler(rhandler);

    gar::IoServicePool::Ptr pool = gar::IoServicePool::instance();
    gar::Server<gar::SocketAdaptor> server(app.priv()->serverOptions(),
                                           pool,
                                           app.priv()->middlewarePipeline(),
                                           app.priv()->resourceHandler());

    gap::Logger::setLogLevel(gap::LogLevel::INFO);
    server.run();

    return 0;
}


