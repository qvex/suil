//
// Created by dc on 6/3/16.
//

#include "util.h"
#include "response.h"
#include "response_internal.h"

namespace gap {

    HttpResponse::HttpResponse() {
        handlers_ = new gar::HttpResponseHandlers;
    }
    HttpResponse::HttpResponse(int code)
        : statusCode_(code) {
        handlers_ = new gar::HttpResponseHandlers;
    }

    HttpResponse::HttpResponse(std::string body)
        : body_(std::move(body)) {
        handlers_ = new gar::HttpResponseHandlers;
    }

    HttpResponse::HttpResponse(json::wvalue&& jsonValue)
        : jsonValue_(std::move(jsonValue)) {
        setHeader("Content-Type", "application/json");
        handlers_ = new gar::HttpResponseHandlers;
    }

    HttpResponse::HttpResponse(int code, std::string body)
        : statusCode_(code),
          body_(std::move(body)) {
        handlers_ = new gar::HttpResponseHandlers;
    }
    HttpResponse::HttpResponse(const json::wvalue& jsonValue)
        : body_(json::dump(jsonValue)) {
        setHeader("Content-Type", "application/json");
        handlers_ = new gar::HttpResponseHandlers;
    }
    HttpResponse::HttpResponse(int code, const json::wvalue& jsonValue)
        : statusCode_(code),
          body_(json::dump(jsonValue)) {
        setHeader("Content-Type", "application/json");
        handlers_ = new gar::HttpResponseHandlers;
    }

    HttpResponse::HttpResponse(HttpResponse&& resp) {
        this->handlers_ = resp.handlers_;
        resp.handlers_ = nullptr;
        *this = std::move(resp);
    }

    HttpResponse::~HttpResponse() {
        if (handlers_ != nullptr) {
            delete (gar::HttpResponseHandlers *)handlers_;
        }
        handlers_ = nullptr;
    }

    HttpResponse& HttpResponse::operator=(HttpResponse&& resp) noexcept {
        handlers_ = resp.handlers_;
        resp.handlers_ = nullptr;
        body_ = std::move(resp.body_);
        jsonValue_ = std::move(resp.jsonValue_);
        statusCode_ = resp.statusCode_;
        headers_ = std::move(resp.headers_);
        completed_ = resp.completed_;
        return *this;
    }

    bool HttpResponse::completed() const noexcept {
        return completed_;
    }

    void HttpResponse::clear() {
        body_.clear();
        jsonValue_.clear();
        statusCode_ = 200;
        headers_.clear();
        completed_ = false;
    }

    void HttpResponse::setHeader(std::string key, std::string value) {
        headers_.erase(key);
        headers_.emplace(std::move(key), std::move(value));
    }
    void HttpResponse::addHader(std::string key, std::string value) {
        headers_.emplace(std::move(key), std::move(value));
    }

    const std::string& HttpResponse::getHeaderValue(const std::string& key) const {
        if (headers_.count(key)) {
            return headers_.find(key)->second;
        }
        return gap::EMPTYSTR;
    }

    void HttpResponse::end() {
        if (!completed_) {
            completed_ = true;
            gar::HttpResponseHandlers *h = handlers();
            if (h && h->completeRequestHandler) {
                h->completeRequestHandler();
            }
        }
    }

    void HttpResponse::end(const std::string& bodyPart) {
        body_ += bodyPart;
        end();
    }

    bool HttpResponse::isAlive() {
        gar::HttpResponseHandlers *h = handlers();
        if (h && h->isAliveChecker()) {
            return h->isAliveChecker();
        }
        return false;
    }

    gar::HttpResponseHandlers *HttpResponse::handlers() {
        return handlers_== nullptr? nullptr: (gar::HttpResponseHandlers*)handlers_;
    }

    const HttpHeaders& HttpResponse::headers() const {
        return headers_;
    }

    std::string& HttpResponse::body() {
        return body_;
    }

    const json::wvalue& HttpResponse::jsonValue() const {
        return jsonValue_;
    }

    int&  HttpResponse::statusCode() {
        return statusCode_;
    }

    void HttpResponse::write(const std::string& bodyPart) {
        body_ += bodyPart;
    }

    void HttpResponse::end(int statusCode, const json::wvalue& jsonWvalue) {
        body_ = json::dump(jsonWvalue);
        setHeader("Content-Type", "application/json");
        end();
    }
}

