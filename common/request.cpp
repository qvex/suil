//
// Created by dc on 6/3/16.
//

#include "util.h"
#include "log.h"
#include "request.h"

namespace gap {

    HttpRequest::HttpRequest()
        : method_(HttpMethod::GET),
          urlParams_{} {
    }

    HttpRequest::HttpRequest(HttpMethod method,
                std::string rawUrl)
        : method_(method),
          rawUrl_(std::move(rawUrl)),
          urlParams_{}
    {
        url_ = rawUrl_.substr(0, rawUrl_.find("?"));
    }

    HttpRequest::HttpRequest(HttpMethod method,
                    std::string rawUrl,
                    std::string url,
                    QueryString urlParams,
                    HttpHeaders headers,
                    std::string body)
        : method_(method),
          rawUrl_(std::move(rawUrl)),
          url_(std::move(url)),
          urlParams_(std::move(urlParams)),
          headers_(std::move(headers)),
          body_(std::move(body))
    {
    }


    HttpMethod      HttpRequest::method() const{
        return method_;
    }
    const std::string&    HttpRequest::rawUrl() const{
        return rawUrl_;
    }
    const std::string&  HttpRequest::url() const {
        return url_;
    }
    const QueryString&  HttpRequest::urlParams() const {
        return urlParams_;
    }
    const HttpHeaders&  HttpRequest::headers() const {
        return headers_;
    }
    std::string&        HttpRequest::body() {
        return body_;
    }
    const std::string   HttpRequest::bodyCopy() const {
        return std::string(body_);
    }

    const std::string&   HttpRequest::getHeaderValue(const std::string& key) const {
        if (headers_.count(key)) {
            return headers_.find(key)->second;
        }
        return gap::EMPTYSTR;
    }
}
