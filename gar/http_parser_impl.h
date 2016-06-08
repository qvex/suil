//
// Created by dc on 6/3/16.
//

#ifndef GAR_HTTP_PARSER_H
#define GAR_HTTP_PARSER_H

#include "util.h"
#include "http_parser.h"
#include "request.h"

namespace gar {

    class HttpParserHandler {
    public:
        virtual void handleHeader() = 0;
        virtual void handleMessage() = 0;
    };


    class HttpPaser {
        typedef std::shared_ptr<HttpPaser> Ptr;
    public:
        static int onMessageBegin(http_parser* parser) {
            HttpPaser *impl = (HttpPaser*)parser->data;
            impl->clear();
            return 0;
        }

        static int onUrl(http_parser* parser, const char* at, size_t len){
            HttpPaser *impl = (HttpPaser*)parser->data;
            impl->rawUrl_.insert(impl->rawUrl_.end(), at, at+len);
            return 0;
        }

        static int onHeaderField(http_parser* parser, const char* at, size_t len) {
            HttpPaser *impl = (HttpPaser*)parser->data;
            switch (impl->headerBuildState_) {
                case 0:
                    if (!impl->headerValue_.empty()) {
                        impl->headers_.emplace(
                                std::move(impl->headerField_),
                                std::move(impl->headerValue_));
                    }
                    impl->headerField_.assign(at, at+len);
                    impl->headerBuildState_ = 1;
                    break;
                case 1:
                    impl->headerField_.insert(impl->headerField_.end(), at, at+len);
                    break;
            }
            return 0;
        }

        static int onHeaderValue(http_parser* parser, const char* at, size_t len) {
            HttpPaser *impl = (HttpPaser*)parser->data;
            switch (impl->headerBuildState_) {
                case 0:
                    impl->headerValue_.insert(impl->headerValue_.end(), at, at+len);
                    break;
                case 1:
                    impl->headerBuildState_ = 0;
                    impl->headerValue_.assign(at, at+len);
                    break;
            }
            return 0;
        }

        static int onHeadersComplete(http_parser* parser) {
            HttpPaser *impl = (HttpPaser*)parser->data;
            if (!impl->headerField_.empty()) {
                impl->headers_.emplace(
                        std::move(impl->headerField_),
                        std::move(impl->headerValue_));
            }
            impl->processHeader();
            return 0;
        }

        static int onBody(http_parser* parser, const char* at, size_t len) {
            HttpPaser *impl = (HttpPaser*)parser->data;
            impl->body_.insert(impl->body_.end(), at, at+len);
            return 0;
        }

        static int onMessageComplete(http_parser* parser) {
            HttpPaser *impl = (HttpPaser*)parser->data;
            impl->url_  = impl->rawUrl_.substr(0, impl->rawUrl_.find("?"));
            impl->urlParams_ = gap::QueryString(impl->rawUrl_);
            impl->processMessage();
            return 0;
        }

        HttpPaser(HttpParserHandler* handler)
            : handler_(handler),
              parser_(new http_parser)
        {
            parser_->data = this;
            http_parser_init(parser_, HTTP_REQUEST);
        }

        ~HttpPaser() {
            if (parser_ != nullptr)
                delete parser_;
            parser_ = nullptr;
        }

        bool data(const char* buffer, int len) {
            static http_parser_settings parserSettings_ {
                    onMessageBegin,
                    onUrl,
                    nullptr,
                    onHeaderField,
                    onHeaderValue,
                    onHeadersComplete,
                    onBody,
                    onMessageComplete
            };

            int nparsed = http_parser_execute(parser_, &parserSettings_, buffer, len);
            return nparsed == len;
        }

        void clear() {
            url_.clear();
            rawUrl_.clear();
            headerBuildState_ = 0;
            headerField_.clear();
            headerValue_.clear();
            headers_.clear();
            urlParams_.clear();
            body_.clear();
        }

        bool complete() {
            return data(nullptr, 0);
        }

        void processHeader() {
            handler_->handleHeader();
        }

        void processMessage() {
            handler_->handleMessage();
        }

        gap::HttpRequest toRequest() const {
            return gap::HttpRequest{(gap::HttpMethod)parser_->method,
                                    std::move(rawUrl_),
                                    std::move(url_),
                                    std::move(urlParams_),
                                    std::move(headers_),
                                    std::move(body_)};
        }

        bool checkVersion(int major, int minor) {
            return parser_->http_major == major && parser_->http_minor == minor;
        }

        const std::string& getHeaderValue(const std::string& key) {
            if (headers_.count(key)) {
                return headers_.find(key)->second;
            }
            return gap::EMPTYSTR;
        }

        const int httpMinor() const {
            return parser_->http_minor;
        }

        const int httpMajor() const {
            return parser_->http_major;
        }

    private:

        http_parser*        parser_;
        HttpParserHandler*  handler_;
        gap::HttpHeaders    headers_;

        std::string         headerField_;
        std::string         headerValue_;
        gap::QueryString    urlParams_;
        std::string         body_;
        std::string         url_;
        std::string         rawUrl_;
        int                 headerBuildState_;
    };
}
#endif //GAR_HTTP_PARSER_H
