//
// Created by dc on 6/3/16.
//

#ifndef GAR_REQUEST_H
#define GAR_REQUEST_H

#include "util.h"
#include "query_string.h"
#include <unordered_map>

namespace gap {

    enum HttpMethod {
        DELETE  = 0x1,
        GET     = 0x2,
        HEAD    = 0X4,
        POST    = 0X8,
        PUT     = 0x10,
        CONNECT = 0x20,
        OPTIONS = 0x40,
        TRACE   = 0x80
    };

    inline HttpMethod operator|(HttpMethod m1, HttpMethod m2) {
        return static_cast<HttpMethod >(static_cast<int>(m1) | static_cast<int>(m2));
    }

    inline HttpMethod operator&(HttpMethod m1, HttpMethod m2) {
        return static_cast<HttpMethod >(static_cast<int>(m1) & static_cast<int>(m2));
    }

    typedef std::unordered_multimap<std::string, std::string, gap::ci_hash, gap::ci_key_eq>   HttpHeaders;
    typedef std::unordered_multimap<std::string, void*, gap::ci_hash, gap::ci_key_eq>         Environment;

    inline std::string methodName(const HttpMethod method) {
        switch (method) {
            case HttpMethod::GET:
                return "GET";
            case HttpMethod::POST:
                return "POST";
            case HttpMethod::DELETE:
                return "DELETE";
            case HttpMethod::PUT:
                return "PUT";
            case HttpMethod::HEAD:
                return "HEAD";
            case HttpMethod::CONNECT:
                return "CONNECT";
            case HttpMethod::TRACE:
                return "TRACE";
            default:
                return "INVALID";
        }
    }

    class HttpRequest {
    public:

        HttpRequest();
        HttpRequest(HttpMethod method,
                    std::string rawUrl);

        HttpRequest(HttpMethod method,
                    std::string rawUrl,
                    std::string url,
                    QueryString urlParams,
                    HttpHeaders headers,
                    std::string body);


        HttpMethod      method() const;
        const std::string&    rawUrl() const;
        const std::string&  url() const;
        const QueryString&  urlParams() const;
        const HttpHeaders&  headers() const;
        std::string&        body();
        const std::string&   getHeaderValue(const std::string& key) const;
        const std::string   bodyCopy() const;
    private:
        HttpMethod      method_;
        std::string     rawUrl_;
        std::string     url_;
        QueryString     urlParams_;
        HttpHeaders     headers_;
        std::string     body_;
    };
};
#endif //GAR_REQUEST_H
