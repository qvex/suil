//
// Created by dc on 6/7/16.
//

#include "route_params.h"

namespace gap {

    ParamValue ParamValue::undefined_(ParamValue(Type::T_UNDEFINED, ""));

    RouteParams::RouteParams()
    {
    }

    const ParamValue& RouteParams::operator[](const std::string& param) {
        auto value = values_.find(param);
        if (value != values_.end()){
            return value->second;
        }
        return ParamValue::undefined();
    }

    void RouteParams::update(const std::string& name, const ParamValue::Type& type, const std::string& value) {
        values_.erase(name);
        values_.emplace(name, ParamValue(type, value));
    }
}