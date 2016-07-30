//
// Created by dc on 6/6/16.
//

#ifndef GAR_ROUTE_RULE_H
#define GAR_ROUTE_RULE_H

#include <string>

namespace gap {

    class ParamValue {
    public:
        enum class Type {
            T_INT,
            T_FLOAT,
            T_UINT,
            T_STRING,
            T_UNDEFINED
        };

        ParamValue(const Type type, const std::string value)
            : type_(type),
              strValue_(value)
        {
            char *endptr = NULL;
            const char *startptr = value.c_str();
            errno = 0;
            switch (type) {
                case Type::T_INT:
                    value_.ivalue = strtol(startptr, &endptr, 10);
                    if (endptr == startptr || errno == ERANGE) {
                        onundefined();
                    }
                    break;
                case Type ::T_FLOAT:
                    value_.fvalue = strtod(startptr, &endptr);
                    if (endptr == startptr || errno == ERANGE) {
                        onundefined();
                    }
                    break;
                case Type::T_UINT:
                    value_.uvalue = strtoul(value.c_str(), &endptr, 10);
                    if (endptr == startptr || errno == ERANGE || value_.ivalue < 0) {
                        onundefined();
                    }
                    break;
                case Type::T_STRING:
                    strValue_ = value;
                    value_.ivalue = strtol(startptr, &endptr, 10);
                    if (endptr == startptr || errno == ERANGE) {
                        value_.ivalue = INT64_MIN;
                    }
                    break;
                default:
                    onundefined();
                    break;
            }
        }

        const int64_t i() const {
#ifdef GAP_PARAM_VALUE_THROW_ON_TYPE
            if (type_ == Type::T_UNDEFINED) {
                throw std::logic_error("error casing undefined to integer");
            }
#endif
            return value_.ivalue;
        }

        const uint64_t u() const {
#ifdef GAP_PARAM_VALUE_THROW_ON_TYPE
            if (type_ == Type::T_UNDEFINED) {
                throw std::logic_error("error casing undefined to integer");
            }
#endif
            return value_.uvalue;
        }

        const double f() const {
#ifdef GAP_PARAM_VALUE_THROW_ON_TYPE
            if (type_ == Type::T_UNDEFINED) {
                throw std::logic_error("error casing undefined to integer");
            }
#endif
            return value_.fvalue;
        }

        const std::string s() const {
#ifdef GAP_PARAM_VALUE_THROW_ON_TYPE
            if (type_ == Type::T_UNDEFINED) {
                throw std::logic_error("error casing undefined to integer");
            }
#endif
            return strValue_;
        }

        const bool isUndefined() const {
            return type_ == Type::T_UNDEFINED;
        }

        const Type type() const {
            return type_;
        }

        const static ParamValue& undefined() {
            return ParamValue::undefined_;
        }

    private:
        void onundefined() {
            type_ = Type::T_UNDEFINED;
            value_.ivalue = INT64_MIN;
            strValue_ = "undefined";
        }

        Type            type_;
        std::string     strValue_;
        union {
            int64_t    ivalue;
            double     fvalue;
            uint64_t   uvalue;
        }              value_;
        static    ParamValue undefined_;
    };
}
#endif //GAR_ROUTE_RULE_H
