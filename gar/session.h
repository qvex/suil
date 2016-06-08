//
// Created by dc on 6/3/16.
//

#ifndef GAR_SESSION_H
#define GAR_SESSION_H

#include "stats.h"
#include "timer_queue.h"
#include "socket_adaptors.h"
#include "resource_handler_priv.h"
#include "http_parser_impl.h"
#include "response_internal.h"
#include "middleware_pipeline_priv.h"

#include <boost/algorithm/string/predicate.hpp>

namespace gar {

#ifndef GAR_READ_BUFFER_SIZE
#define GAR_READ_BUFFER_SIZE    4096
#endif
    static const std::string EXPECT_100_CONTINUE = "HTTP/1.1 100 Continue\r\n\r\n";
    static const std::string HTTP_HEADER_SEP = ": ";
    static const std::string HTTP_CRLF = "\r\n";

    static const std::unordered_map<int, std::string> STATUS_CODES = {
            {200, "HTTP/1.1 200 OK\r\n"},
            {201, "HTTP/1.1 201 Created\r\n"},
            {202, "HTTP/1.1 202 Accepted\r\n"},
            {204, "HTTP/1.1 204 No Content\r\n"},

            {300, "HTTP/1.1 300 Multiple Choices\r\n"},
            {301, "HTTP/1.1 301 Moved Permanently\r\n"},
            {302, "HTTP/1.1 302 Moved Temporarily\r\n"},
            {304, "HTTP/1.1 304 Not Modified\r\n"},

            {400, "HTTP/1.1 400 Bad Request\r\n"},
            {401, "HTTP/1.1 401 Unauthorized\r\n"},
            {403, "HTTP/1.1 403 Forbidden\r\n"},
            {404, "HTTP/1.1 404 Not Found\r\n"},

            {500, "HTTP/1.1 500 Internal Server Error\r\n"},
            {501, "HTTP/1.1 501 Not Implemented\r\n"},
            {502, "HTTP/1.1 502 Bad Gateway\r\n"},
            {503, "HTTP/1.1 503 Service Unavailable\r\n"},
    };

    template <typename Adaptor>
    class Session : public HttpParserHandler {
        typedef std::shared_ptr<Session<Adaptor>> SelfPtr;
    public:

        Session(std::shared_ptr<boost::asio::io_service> ioService,
                    typename Adaptor::Context* context,
                    Stats::Ptr stats,
                    TimerQueue::Ptr timerQueue,
                    gap::MiddlewarePipelinePriv::Ptr mwPipeline,
                    gap::ResourceHandlerPriv::Ptr   resourceHandler)
                : adaptor_(ioService, context),
                  stats_(stats),
                  reading_(false),
                  writing_(false),
                  parser_(this),
                  timerQueue_(timerQueue),
                  mwPipeline_(mwPipeline),
                  resourceHandler_(resourceHandler)
        {
        }

        ~Session() {
            cancelDeadlineTimer();
            GAR_STATS_DECR(stats_, activeConnections);
            GAP_Trace("Session close [%p] (active=%u, total=%u)\n",
                    this, unsigned(stats_->activeConnections), unsigned(stats_->totalConnections));
        }

        Adaptor& adaptor() {
            return adaptor_;
        }

        void start() {
            GAR_STATS_INCR(stats_, activeConnections);
            GAR_STATS_INCR(stats_, totalConnections);
            GAP_Trace("[this:%p] session open (active=%u, total=%u)\n",
                    this, unsigned(stats_->activeConnections), unsigned(stats_->totalConnections));

            adaptor_.start([this](const Boost_Err& err) {
                if (!err) {
                    startDeadline();
                    read();
                } else {
                    endSession();
                }
            });
        }

        virtual void handleHeader() {
            if (parser_.checkVersion(1,1) && parser_.getHeaderValue("expect") == "100-continue") {
                obuffers_.clear();
                obuffers_.emplace_back(EXPECT_100_CONTINUE.data(), EXPECT_100_CONTINUE.size());
                write();
            }
        }

        virtual void handleMessage() {
            cancelDeadlineTimer();
            req_ = std::move(parser_.toRequest());
            gap::HttpRequest& req = req_;
            addKeepAlive_ = false;
            bool invalidRequest = false;

            if (parser_.checkVersion(1, 0)) {
                if (boost::iequals(req.getHeaderValue("connection"), "Keep-Alive")) {
                    addKeepAlive_ = true;
                } else {
                    closeConnection_ = true;
                }
            } else if(parser_.checkVersion(1, 1)) {
                std::string connHeader = req.getHeaderValue("connection");
                if (boost::iequals(connHeader,"close")) {
                    closeConnection_ = true;
                } else if(boost::iequals(connHeader, "Keep-Alive")){
                    addKeepAlive_ = true;
                }

                if (!req.headers().count("host")) {
                    invalidRequest = true;
                    res_ = gap::HttpResponse(400);
                }
            }

            GAP_Trace("Request(%d): %s [%p] HTTP/%d.%d %s %s\n", invalidRequest,
                boost::lexical_cast<std::string>(adaptor_.remoteEndpoint()).c_str(),
                this, parser_.httpMajor(), parser_.httpMinor(),
                gap::methodName(req.method()).c_str(), req.url().c_str());

            if (!invalidRequest) {
                res_.handlers()->completeRequestHandler = []{ };
                res_.handlers()->isAliveChecker = [this]()->bool {return adaptor_.isOpen(); };

                // Call middleware pipeline
                callStack_.clear();
                mwPipeline_->callBeforeHandlers(callStack_, env_, req, res_);

                if (!res_.completed()) {
                    res_.handlers()->completeRequestHandler = [this]{ this->completeRequest(); };
                    callPipelineAfter_ = true;
                    resourceHandler_->handle(req, res_);

                    if (addKeepAlive_) {
                        res_.setHeader("connection", "Keep-Alive");
                    }
                } else {
                    completeRequest();
                }
            } else {
                completeRequest();
            }
        }

        void completeRequest() {

            GAP_Trace("Response: [%p] %s %d %d\n", this, req_.rawUrl().c_str(), res_.statusCode(), closeConnection_);
            if(callPipelineAfter_) {
                callPipelineAfter_ = false;
                // Call middleware after handlers
                mwPipeline_->callAfterHanders(callStack_, env_, req_, res_);
                callStack_.clear();
            }

            res_.handlers()->completeRequestHandler = nullptr;
            if (!adaptor_.isOpen()) {
                return;
            }

            obuffers_.clear();
            obuffers_.reserve(4*(res_.headers().size()+5)+3);
            if (res_.body().empty() && res_.jsonValue().t() == gap::json::type::Object) {
                res_.body() = gap::json::dump(res_.jsonValue());
            }

            if (!STATUS_CODES.count(res_.statusCode())) {
                res_.statusCode() = 500;
            }
            {
                auto& status = STATUS_CODES.find(res_.statusCode())->second;
                obuffers_.emplace_back(status.data(), status.size());
            }

            if (res_.statusCode() > 400 && res_.body().empty())
                res_.body() = STATUS_CODES.find(res_.statusCode())->second.substr(9);

            for(auto& kv : res_.headers()) {
                obuffers_.emplace_back(kv.first.data(), kv.first.size());
                obuffers_.emplace_back(HTTP_HEADER_SEP.data(), HTTP_HEADER_SEP.size());
                obuffers_.emplace_back(kv.second.data(), kv.second.size());
                obuffers_.emplace_back(HTTP_CRLF.data(), HTTP_CRLF.size());
            }

            if(!res_.headers().count("content-length")) {
                contentLength_ = std::to_string(res_.body().size());
                static std::string CONTENTLEN_TAG = "Content-Length: ";
                obuffers_.emplace_back(CONTENTLEN_TAG.data(), CONTENTLEN_TAG.size());
                obuffers_.emplace_back(contentLength_.data(), contentLength_.size());
                obuffers_.emplace_back(HTTP_CRLF.data(), HTTP_CRLF.size());
            }

            if(!res_.headers().count("server")) {
                static std::string SERVER_TAG = "Server: ";
                obuffers_.emplace_back(SERVER_TAG.data(), SERVER_TAG.size());
                obuffers_.emplace_back(serverName_.data(), serverName_.size());
                obuffers_.emplace_back(HTTP_CRLF.data(), HTTP_CRLF.size());
            }

            if(!res_.headers().count("date")) {
                static std::string DATE_TAG = "Date: ";
                obuffers_.emplace_back(DATE_TAG.data(), DATE_TAG.size());
                obuffers_.emplace_back(dateTimeStr_.data(), dateTimeStr_.size());
                obuffers_.emplace_back(HTTP_CRLF.data(), HTTP_CRLF.size());
            }

            if(addKeepAlive_) {
                static std::string KEEPALIVE_TAG = "Connection: Keep-Alive";
                obuffers_.emplace_back(KEEPALIVE_TAG.data(), KEEPALIVE_TAG.size());
                obuffers_.emplace_back(HTTP_CRLF.data(), HTTP_CRLF.size());
            }

            obuffers_.emplace_back(HTTP_CRLF.data(), HTTP_CRLF.size());
            resBodyCopy_.swap(res_.body());
            obuffers_.emplace_back(resBodyCopy_.data(), resBodyCopy_.size());

            write();
            if (readAfterWrite_) {
                readAfterWrite_ = false;
                startDeadline();
                read();
            }
        }

    private:
        void read() {
            reading_ = true;
            adaptor_.socket().async_read_some(boost::asio::buffer(ibuffer_),
                              boost::bind(&Session::readHandler,
                                          this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
        }

        void write() {
            writing_ = true;
            boost::asio::async_write(adaptor_.socket(), obuffers_,
                                     boost::bind(&Session::writeHandler,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred));
        }

        void readHandler(const Boost_Err &err, std::size_t received) {
            GAR_STATS_ADD(stats_, receivedBytes, received);
            bool errorWhileReading = true;
            if (!err) {
                bool status = parser_.data(ibuffer_.data(), received);
                if (status && adaptor_.isOpen() && !closeConnection_) {
                    errorWhileReading = false;
                }
            } else {
                GAP_Dbg("[this:%p] error during read: %s\n", this, err.message().c_str());
            }

            if (errorWhileReading) {
                cancelDeadlineTimer();

                parser_.complete();
                adaptor_.close();
                reading_ = false;
                GAP_Trace("[this:%p] session end %s\n", this, __FUNCTION__);
                endSession();
            } else if (!callPipelineAfter_) {
                startDeadline();
                read();
            } else {
                readAfterWrite_ = true;
            }
        }

        void writeHandler(const Boost_Err &err, std::size_t sent) {
            GAR_STATS_ADD(stats_, sentBytes, sent);

            writing_ = false;
            res_.clear();
            resBodyCopy_.clear();

            if (!err) {
                if (closeConnection_) {
                    adaptor_.close();
                    GAP_Trace("[this:%p] session end(1) %s\n", this, __FUNCTION__);
                    endSession();
                }
            } else {
                GAP_Trace("[this:%p] session end(2) %s\n", this, __FUNCTION__);
                endSession();
            }
        }

        void endSession() {
            GAP_Trace("[%p] session ending? reading=%d writing=%d\n", this, reading_, writing_);
            if (!reading_ && !writing_) {
                delete this;
            }
        }

        void cancelDeadlineTimer() {
            GAP_Trace("[this:%p] deadline timer canceled: [timer:%p, step:%d]\n",
                        this, timerCancelKey_.first, timerCancelKey_.second);
            timerQueue_->cancel(timerCancelKey_);
        }

        void startDeadline(int timeout = 5) {
            cancelDeadlineTimer();
            timerCancelKey_ = timerQueue_->add([this] {
               if (adaptor_.isOpen()) {
                   adaptor_.close();
                   GAP_Dbg("[this:%p] connection timed out\n", this);
               }
            });

            GAP_Trace("[this:%p] deadline timer added: [timer:%p, step:%d]\n",
                        this, timerCancelKey_.first, timerCancelKey_.second);
        }

    private:
        Adaptor                 adaptor_;
        boost::array<char, GAR_READ_BUFFER_SIZE>    ibuffer_;
        std::vector<boost::asio::const_buffer>      obuffers_;
        bool                    reading_{false};
        bool                    writing_{false};
        bool                    addKeepAlive_{false};
        bool                    closeConnection_{false};
        bool                    callPipelineAfter_{false};
        bool                    readAfterWrite_{false};

        std::string             contentLength_;
        std::string             resBodyCopy_;
        std::string             serverName_;
        std::string             dateTimeStr_;

        Stats::Ptr              stats_;
        HttpPaser               parser_;
        gap::HttpRequest        req_;
        gap::HttpResponse       res_;
        TimerQueue::Ptr         timerQueue_;
        TimerQueue::Key         timerCancelKey_;
        gap::MiddlewarePipelinePriv::Ptr mwPipeline_;
        gap::MiddlewarePipelinePriv::CallStack callStack_;
        gap::Environment            env_;
        gap::ResourceHandlerPriv::Ptr   resourceHandler_;
    };
}
#endif //GAR_SESSION_H
