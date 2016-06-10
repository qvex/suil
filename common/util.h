//
// Created by dc on 6/4/16.
//

#ifndef GAR_UTIL_H
#define GAR_UTIL_H

#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>

namespace gap {
    const std::string EMPTYSTR = "";

    //--------------------------------------------------------------------
    // https://github.com/dccarter/crow/blob/master/include/ci_map.h
    //--------------------------------------------------------------------
    struct ci_hash
    {
        size_t operator()(const std::string& key) const
        {
            std::size_t seed = 0;
            std::locale locale;

            for(auto c : key)
            {
                boost::hash_combine(seed, std::toupper(c, locale));
            }

            return seed;
        }
    };

    struct ci_key_eq
    {
        bool operator()(const std::string& l, const std::string& r) const
        {
            return boost::iequals(l, r);
        }
    };
    //--------------------------------------------------------------------
    // https://github.com/dccarter/crow/blob/master/include/ci_map.h
    //--------------------------------------------------------------------

#define GAP_assert(cond, fmt, ...) if (!(cond)) printf(fmt "\n", ## __VA_ARGS__); assert(cond)
}
#endif //GAR_UTIL_H
