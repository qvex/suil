//
// Created by dc on 6/6/16.
//

#include <regex>
#include <iostream>
#include <assert.h>
#include "trie.h"
#include <unordered_map>


#include "route_params_parser.h"

int main(int argc, char* argv[])
{


    gap::RouteParamsParser parser(gap::RouteParamsParser::compile(
            "api/users/{id:int}/{name:string}/{id2:float}/{name2:string}"));

    bool status;
    gap::RouteParams params = parser.build("/api/users/400/carter/600/mpho", status);
    if (status)
    printf("id=%d name=%s, id2=%f name2=%s\n",
            params["id"].i(), params["name"].s().c_str(), params["id2"].f(), params["name2"].s().c_str());

    return 0;
}



