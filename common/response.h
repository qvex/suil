//
// Created by dc on 6/3/16.
//

#ifndef GAR_HTTPRESPONSE_H
#define GAR_HTTPRESPONSE_H

#include "request.h"
#include "json.h"

namespace gar {
    struct HttpResponseHandlers;
}
namespace gap {


    class HttpResponse {
    public:
        gar::HttpResponseHandlers *handlers();

        HttpResponse();
        explicit HttpResponse(int code);
        HttpResponse(std::string body);
        HttpResponse(json::wvalue&& jsonValue);
        HttpResponse(int code, std::string body);
        HttpResponse(const json::wvalue& jsonValue);
        HttpResponse(int code, const json::wvalue& jsonValue);
        HttpResponse(HttpResponse&& resp);
        ~HttpResponse();

        HttpResponse& operator=(const HttpResponse& resp) = delete;
        HttpResponse& operator=(HttpResponse&& resp) noexcept ;

        bool completed() const noexcept;
        void clear();

        void setHeader(std::string key, std::string value);
        void addHader(std::string key, std::string value);
        const std::string& getHeaderValue(const std::string& key) const;
        void end();
        void end(const std::string& bodyPart);
        bool isAlive();
        int&  statusCode();
        const HttpHeaders& headers() const;
        std::string& body();
        const json::wvalue& jsonValue() const;
        void write(const std::string& bodyPart);
        void end(int statusCode, const json::wvalue& jsonWvalue);
        void end(int statusCode, const std::string data);
    private:
        int             statusCode_;
        std::string     body_;
        json::wvalue    jsonValue_;
        HttpHeaders     headers_;
        bool            completed_{false};
        void            *handlers_;
    };
}
#endif //GAR_HTTPRESPONSE_H
