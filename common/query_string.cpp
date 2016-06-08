//
// Created by dc on 6/3/16.
//

#include "query_string_ext.h"
#include "query_string.h"

namespace  gap {

    QueryString::QueryString() {
    }

    QueryString::QueryString(const QueryString &qs)
            : url_(qs.url_) {
        for (auto p:qs.key_value_pairs_) {
            key_value_pairs_.push_back((char *) (p - qs.url_.c_str() + url_.c_str()));
        }
    }

    QueryString &QueryString::operator=(const QueryString &qs) {
        url_ = qs.url_;
        key_value_pairs_.clear();
        for (auto p:qs.key_value_pairs_) {
            key_value_pairs_.push_back((char *) (p - qs.url_.c_str() + url_.c_str()));
        }
        return *this;
    }

    QueryString &QueryString::operator=(QueryString &&qs) {
        key_value_pairs_ = std::move(qs.key_value_pairs_);
        char *old_data = (char *) qs.url_.c_str();
        url_ = std::move(qs.url_);
        for (auto &p:key_value_pairs_) {
            p += (char *) url_.c_str() - old_data;
        }
        return *this;
    }


    QueryString::QueryString(std::string url)
            : url_(std::move(url)) {
        if (url_.empty())
            return;

        key_value_pairs_.resize(MAX_KEY_VALUE_PAIRS_COUNT);

        int count = qs_parse(&url_[0], &key_value_pairs_[0], MAX_KEY_VALUE_PAIRS_COUNT);
        key_value_pairs_.resize(count);
    }

    void QueryString::clear() {
        key_value_pairs_.clear();
        url_.clear();
    }

    std::ostream &operator<<(std::ostream &os, const QueryString &qs) {
        os << "[ ";
        for (size_t i = 0; i < qs.key_value_pairs_.size(); ++i) {
            if (i)
                os << ", ";
            os << qs.key_value_pairs_[i];
        }
        os << " ]";
        return os;

    }

    char *QueryString::get(const std::string &name) const {
        char *ret = qs_k2v(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size());
        return ret;
    }

    std::vector<char *> QueryString::get_list(const std::string &name) const {
        std::vector<char *> ret;
        std::string plus = name + "[]";
        char *element = nullptr;

        int count = 0;
        while (1) {
            element = qs_k2v(plus.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++);
            if (!element)
                break;
            ret.push_back(element);
        }
        return ret;
    }
}
