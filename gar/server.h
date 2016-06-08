//
// Created by dc on 6/4/16.
//

#ifndef GAR_SERVER_H
#define GAR_SERVER_H

#include <future>
#include "util.h"
#include "ioservice_pool.h"
#include "session.h"
#include "server_options.h"
#include "middleware_pipeline_priv.h"
#include "resource_handler_priv.h"

namespace gar {
    using boost::asio::ip::tcp;

    template <typename Adaptor>
    class Server {
    public:

        Server(gap::ServerOptions& options,
               IoServicePool::Ptr pool,
               gap::MiddlewarePipelinePriv::Ptr mwPipeline,
               gap::ResourceHandlerPriv::Ptr resourceHandler)
            : options_(options),
              ioServicePool_(pool),
              ioService_(pool->reserved()),
              acceptor_(*pool->reserved(),
                        tcp::endpoint(boost::asio::ip::address::from_string(options.bindAddress), options.port)),
              signals_{*pool->reserved(), SIGINT, SIGTERM},
              stats_(Stats::Ptr(new Stats)),
              mwPipeline_(mwPipeline),
              resourceHandler_(resourceHandler)
        {
            memset(stats_.get(), 0, sizeof(Stats));
        }

        void run() {
            uint concurrency = ioServicePool_->size();
            timerQueuePool_.reserve(concurrency);
            getCachedDateStrPool_.reserve(concurrency);

            std::future<void> fVoid;

            for (uint i =0; i < concurrency; i++) {
                auto last = std::chrono::steady_clock::now();

                std::string dateStr;
                auto updateDateStrFn = [&] {
                    auto lastTime = time(0);
                    tm lastTm;

                    gmtime_r(&lastTime, &lastTm);
                    dateStr.resize(100);
                    size_t ndateStr = strftime(&dateStr[0], 99, "%a, %d %b %Y %H:%M:%s GMT", &lastTm);
                    dateStr.reserve(ndateStr);
                };

                updateDateStrFn();
                ServicePtr service = ioServicePool_->select(i);
                auto updateDateFn = [&]() -> std::string {
                    if (std::chrono::steady_clock::now() - last > std::chrono::seconds(1)) {
                        last = std::chrono::steady_clock::now();
                        updateDateStrFn();
                    }
                    return dateStr;
                };
                getCachedDateStrPool_.emplace(service.get(), updateDateFn);

                TimerQueue::Ptr timerQueue(new TimerQueue(service.get()));
                timerQueuePool_.emplace(service.get(), timerQueue);
                timerQueue->asyncStart();
            }

            // start the services on the background;
            ioServicePool_->asyncRun([](){});

            signals_.async_wait([&](const Boost_Err& err, int signalNumber) {
                stop();
            });

            GAP_Info("%s server started bound to local port %u\n",
                     options_.serverName.c_str(), options_.port);

            // Start accepting connection
            accept();

            std::thread t([this]{
               ioService_->run();
                // Waits until stopped
                GAP_Info("%s exiting\n", options_.serverName.c_str());
            });
            t.join();
        }

        void stop() {
            GAP_Dbg("[this:%p] Server exiting\n", this);
            GAP_Dbg("Stats: received=%uB sent=%uB totalConnections=%u\n",
                    unsigned(stats_->receivedBytes),
                    unsigned(stats_->sentBytes),
                    unsigned(stats_->totalConnections));
            ioServicePool_->stop();
            ioService_->stop();
        }

    private:

        void accept() {
            ServicePtr is = ioServicePool_->schedule();
            TimerQueue::Ptr tq = timerQueuePool_.find(is.get())->second;
            auto sess = new Session<Adaptor>(is, nullptr, stats_, tq, mwPipeline_, resourceHandler_);
            // TODO: Correctly initialize Session
            acceptor_.async_accept(sess->adaptor().rawSocket(),
            [this, sess, is](Boost_Err err){
                if (!err) {
                    is->post([sess]{
                       sess->start();
                    });
                }
                accept();
            });
        }

    private:
        template<typename Entry>
        using IosKeyedMap = std::unordered_map<boost::asio::io_service*, Entry>;

        ServicePtr                                  ioService_;
        IoServicePool::Ptr                          ioServicePool_;
        IosKeyedMap<TimerQueue::Ptr>                timerQueuePool_;
        IosKeyedMap<std::function<std::string()>>   getCachedDateStrPool_;
        tcp::acceptor                               acceptor_;
        boost::asio::signal_set                     signals_;
        Stats::Ptr                                  stats_;
        gap::ServerOptions&                         options_;
        gap::MiddlewarePipelinePriv::Ptr            mwPipeline_;
        gap::ResourceHandlerPriv::Ptr                   resourceHandler_;
    };
}
#endif //GAR_SERVER_H
