//
// Created by dc on 6/6/16.
//

#ifndef GAR_ROUTER_H
#define GAR_ROUTER_H

#include <unordered_map>
#include "util.h"
#include "param_value.h"

UT_FRIEND_DECLARE(RouteParamsTest);
namespace gap {

    class RouteParamsParser;
    class RouteParams {
    public:
        const ParamValue &operator[](const std::string& param);

    private:
        friend class RouteParamsParser;
        UT_FRIEND(::RouteParamsTest);
        RouteParams();
        void update(const std::string& name, const ParamValue::Type& type, const std::string& value);
        typedef std::unordered_map<std::string, ParamValue> ValuesMap;
        ValuesMap       values_;
    };
}
#endif //GAR_ROUTER_H
