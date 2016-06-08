//
// Created by dc on 6/6/16.
//

#ifndef GAR_ROUTE_RULE_H
#define GAR_ROUTE_RULE_H

#include <functional>

#include "request.h"
#include "response.h"

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
              strValue_("")
        {
            switch (type) {
                case Type::T_INT:
                    value_.ivalue = atoi(value.c_str());
                    break;
                case Type ::T_FLOAT:
                    value_.fvalue = atof(value.c_str());
                    break;
                case Type::T_UINT:
                    value_.uvalue = strtoul(value.c_str(), 0, 10);
                    break;
                case Type::T_STRING:
                    strValue_ = value;
                    break;
                default:
                    strValue_ = "undefined";
                    type_ = Type::T_UNDEFINED;
                    break;
            }
        }

        const int i() const {
            return value_.ivalue;
        }

        const unsigned u() const {
            return value_.uvalue;
        }

        const double f() const {
            return value_.fvalue;
        }

        const std::string s() const {
            return strValue_;
        }

        const static ParamValue& undefined() {
            return ParamValue::undefined_;
        }

    private:
        Type            type_;
        std::string     strValue_;
        union {
            int         ivalue;
            float       fvalue;
            unsigned    uvalue;
        }               value_;
        static    ParamValue undefined_;
    };
}
#endif //GAR_ROUTE_RULE_H
