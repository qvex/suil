//
// Created by dc on 6/3/16.
//

#ifndef GAR_STATS_H
#define GAR_STATS_H

#include <memory>
#include <atomic>

namespace gar {

    struct Stats : public std::enable_shared_from_this<Stats> {
        typedef std::shared_ptr<Stats> Ptr;
        std::atomic_uint        totalConnections;
        std::atomic_uint        activeConnections;
        std::atomic_uint        sentBytes;
        std::atomic_uint        receivedBytes;
    };

#if defined(GAR_DEBUG) && defined(GAR_STATS_ENABLED)
#define GAR_STATS_INCR(stats, field)   stats->field++
#define GAR_STATS_DECR(stats, field)   stats->field--
#define GAR_STATS_ADD(stats, field, addend) stats->field += addend
#define GAR_STATS_SUB(stats, field, addend) stats->field += addend
#else
#define GAR_STATS_INCR(stats, field)
#define GAR_STATS_DECR(stats, field)
#define GAR_STATS_ADD(stats, field, addend)
#define GAR_STATS_SUB(stats, field, addend)
#endif
}
#endif //GAR_STATS_H
