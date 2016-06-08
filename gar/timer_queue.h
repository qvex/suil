//
// Created by dc on 6/4/16.
//

#ifndef GAR_TIMER_QUEUE_H
#define GAR_TIMER_QUEUE_H

#include <memory>
#include <boost/asio.hpp>
#include <deque>
#include <functional>
#include <thread>
#include <chrono>

#include "log.h"

namespace gar {

    class TimerQueue : public std::enable_shared_from_this<TimerQueue> {
    public:
        typedef std::pair<TimerQueue*, int> Key;
        typedef std::shared_ptr<TimerQueue> Ptr;
        void cancel(Key& k) {
            auto self = k.first;
            k.first = nullptr;
            if (!self) return;

            unsigned int index = (unsigned int) (k.second - self->step_);
            if (index < self->dq_.size())
                self->dq_[index].second = nullptr;
        }

        Key add(std::function<void()> cb) {
            dq_.emplace_back(std::chrono::steady_clock::now(), std::move(cb));
            int ret = step_ + dq_.size()-1;
            GAP_Trace("Added timer [%p] ret = %d\n", this, ret);
            return {this, ret};
        }

        void process()
        {
            if (!ioService_) return;
            auto now = std::chrono::steady_clock::now();

            while(!dq_.empty()) {
                auto& front = dq_.front();
                if (now - front.first < std::chrono::seconds(tick_))
                    break;

                if (front.second) {
                    GAP_Dbg("[this:%p] invoking timer callback step=%d\n", this, step_);
                    front.second();
                }
                dq_.pop_front();
                step_++;
            }
        }

        void asyncStart() {
            timer_.expires_from_now(boost::posix_time::seconds(10));
            timer_.async_wait(boost::bind(&TimerQueue::timerHandler, shared_from_this(), boost::asio::placeholders::error));
            GAP_Dbg("[this:%p] Timer queue started\n", this);
        }

        TimerQueue(boost::asio::io_service* ioService)
            : ioService_(ioService),
              timer_(*ioService, boost::posix_time::seconds(1))
        {
        }

    private:
        void timerHandler(const boost::system::error_code& err) {
            if (!err) {
                process();
                timer_.expires_from_now(boost::posix_time::seconds(1));
                timer_.async_wait(boost::bind(&TimerQueue::timerHandler, shared_from_this(), boost::asio::placeholders::error));
            } else {
                GAP_Dbg("[this:%p] %s error: %s\n", this, __FUNCTION__, err.message().c_str());
            }
        }

        int                     tick_{5};
        boost::asio::io_service *ioService_;
        int                     step_{0};
        std::deque<std::pair<decltype(std::chrono::steady_clock::now()), std::function<void()>>> dq_;
        boost::asio::deadline_timer timer_;
    };
}

#endif //GAR_TIMER_QUEUE_H
