//
// Created by dc on 6/6/16.
//

#ifndef GAR_ROUTER_H
#define GAR_ROUTER_H

#include "param_value.h"
#include "resource_handler.h"
#include "trie.h"

namespace gap {

    class RouteParamsParser;
    class RouteParams {
    public:
        const ParamValue &operator[](const std::string& param);

    private:
        friend class RouteParamsParser;
        RouteParams();
        void update(const std::string& name, const ParamValue::Type& type, const std::string& value);
        typedef std::unordered_map<std::string, ParamValue> ValuesMap;
        ValuesMap       values_;
    };
}
#endif //GAR_ROUTER_H
