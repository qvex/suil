//
// Created by dc on 6/7/16.
//
#include "util.h"
#include "route_params_parser.h"

namespace gap {

    RouteParamsParser RouteParamsParser::parser_ = RouteParamsParser();
    RouteParamsParser::ParseContext RouteParamsParser::ctx_ = RouteParamsParser::ParseContext();

    RouteParamsParser::RouteParamsParser(RouteParamsParser&& other) {
        this->regex_ = other.regex_;
        this->paramDefs_ = std::move(other.paramDefs_);
        this->prefix_ = std::move(other.prefix_);
    }
    RouteParamsParser& RouteParamsParser::operator=(RouteParamsParser&& other) {
        this->regex_ = other.regex_;
        this->paramDefs_ = std::move(other.paramDefs_);
        this->prefix_ = std::move(other.prefix_);
        return *this;
    }

    const std::string& RouteParamsParser::prefix() const {
        return prefix_;
    }

    RouteParams RouteParamsParser::build(const std::string& resource, bool& status) {
        RouteParams params;
        std::smatch result;
        status = false;
        if (std::regex_search(resource, result, regex_)) {
            params.values_.clear();
            if (paramDefs_.size() == result.size()-1) {
                for(int i=0; i < paramDefs_.size(); i++) {
                    auto p = paramDefs_[i];
                    params.update(p.first, p.second, result[i+1].str());
                }
                status = true;
            }
        }
        return params;
    }

    RouteParamsParser RouteParamsParser::compile(const std::string& route) {
        // acquire the parser first.
        ctx_.acquire();
        parser_.paramDefs_.clear();
        parser_.prefix_.clear();

        std::string regexStr;
        for(char c: route) {
            process(c);
        }

        // iterate over the parameters and create a regex str
        for(auto& p : parser_.paramDefs_) {
            if (p.second == ParamValue::Type::T_INT) {
                regexStr += "/(-?\\d+)";
            } else if(p.second == ParamValue::Type::T_FLOAT) {
                regexStr += "/(-?\\d+(?:\\.\\d+)?)";
            } else if(p.second == ParamValue::Type::T_STRING) {
                regexStr += "/(\\w+)";
            } else if(p.second == ParamValue::Type::T_UINT) {
                regexStr += "/(\\d+)";
            } else {
                GAP_assert(false, "ERROR: Unreachable code, contact developers");
            }
        }
        regexStr = "^" + regexStr + "$";

        if (parser_.prefix_.empty() && !ctx_.prefix_.empty()) {
            parser_.prefix_ = std::move(ctx_.prefix_);
        }
        parser_.regex_ = std::regex(std::move(regexStr));
        ctx_.release();

        // move the parser
        return std::move(parser_);
    }

    void RouteParamsParser::process(const char& c) {
        switch (ctx_.state_) {
            case '{':
                if(c == ':') {
                    GAP_assert(!ctx_.name_.empty(), "ERROR: empty parameter names not allowed");
                    ctx_.state_ = ':';
                    ctx_.type_.clear();
                } else {
                    if (ctx_.name_.empty()) {
                        GAP_assert(isalpha(c), "ERROR: parameter names can only start with alphabetical letters");
                    } else {
                        GAP_assert(isalnum(c), "ERROR: parameter names can only contain letters and numbers");
                    }
                    ctx_.name_ += c;
                }
                break;
            case ':':
                if (c == '}') {
                    ParamValue::Type ptype = validateType();
                    GAP_assert((ptype != ParamValue::Type::T_UNDEFINED),
                               "ERROR: unsupported type, only float, int, string and uint are supported: %s", ctx_.type_.c_str());
                    parser_.paramDefs_.emplace_back(std::move(ctx_.name_), std::move(ptype));
                    ctx_.state_ = '}';
                } else {
                    ctx_.type_ += c;
                }
                break;

            case '}':
                GAP_assert((c == '/'),  "ERROR: parameters should be followed by either \'/\' or nothing");
                ctx_.clear();
                break;
            case '/':
                if (ctx_.prefixing_) {
                    if (c == '{') {
                        ctx_.prefixing_ = false;
                        ctx_.state_ = '{';
                        parser_.prefix_ = std::move(ctx_.prefix_);
                        parser_.prefix_.pop_back();
                        ctx_.prefix_.clear();
                    } else {
                        ctx_.prefix_ += c;
                    }
                } else {
                    GAP_assert((c == '{'), "ERROR: Only named parameters are allowed in a route");
                    ctx_.state_ = '{';
                };
                break;
            default:
                GAP_assert(false, "ERROR unreachable code, contact developer: %c", c);
                break;
        }
    }

    void RouteParamsParser::dump() {
    }

    ParamValue::Type RouteParamsParser::validateType() {
        std::regex r("int|float|string|uint");
        if (std::regex_match(ctx_.type_, r)) {
            if(ctx_.type_ == "int") {
                return ParamValue::Type::T_INT;
            } else if(ctx_.type_ == "float") {
                return ParamValue::Type::T_FLOAT;
            } else if(ctx_.type_ == "string") {
                return ParamValue::Type::T_STRING;
            } else if(ctx_.type_ == "uint") {
                return ParamValue::Type::T_UINT;
            }
        }
        return ParamValue::Type::T_UNDEFINED;
    }
}
