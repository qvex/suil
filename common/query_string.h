//
// Created by dc on 6/3/16.
//

#ifndef GAR_QUERY_STRING_H
#define GAR_QUERY_STRING_H

#include <vector>
#include <string>
#include <ostream>

namespace gap {

    class QueryString {

    public:
        static const int MAX_KEY_VALUE_PAIRS_COUNT = 256;

        QueryString();

        QueryString(const QueryString &qs);

        QueryString &operator=(const QueryString &qs);

        QueryString &operator=(QueryString &&qs);

        QueryString(std::string url);

        void clear();

        friend std::ostream &operator<<(std::ostream &os, const QueryString &qs);

        char *get(const std::string &name) const;

        std::vector<char *> get_list(const std::string &name) const;

    private:
        std::string url_;
        std::vector<char *> key_value_pairs_;
    };
}

#endif //GAR_QUERY_STRING_H
