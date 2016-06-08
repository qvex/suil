//
// Created by dc on 6/2/16.
//

#include "ioservice_pool.h"
#include "log.h"

namespace gar {

    IoServicePool::Ptr IoServicePool::inst_ = nullptr;

    IoServicePool::IoServicePool(const uint poolSize)
        : poolSize_{poolSize},
          nextService_(0),
          serviceSet_({}),
          workSet_({}),
          threadSet_({})
    {
        io_service *service;
        for (int i=0; i < poolSize_; i++) {
            service = new io_service();
            serviceSet_.emplace_back(0, ServicePtr(service));
            workSet_.emplace_back(new io_service::work(*service));
        }
        server_ = ServicePtr(new io_service);
        GAP_Dbg("IoServicePool initialized with %u services\n", poolSize_);
    }

    ServicePtr IoServicePool::select(const uint index) {
        if (index < poolSize_) {
            serviceSet_[index].first++;
            return serviceSet_[index].second;
        }
        throw std::invalid_argument("Argument (index) out of range exception");
    }

    ServicePtr  IoServicePool::reserved() {
        return server_;
    }

    ServicePtr IoServicePool::schedule() {
        auto next = std::min_element(serviceSet_.begin(), serviceSet_.end(),
                            [](WeightedService& s1, WeightedService& s2){
                                return s1.second.use_count() < s2.second.use_count()
                                         || s1.first < s2.first;
                            });
        next->first++;
        GAP_Trace("Selected [%p] service (%u, %u)\n", next->second.get(), next->second.use_count(), next->first);
        return  next->second;
    }

    void IoServicePool::run() {
        asyncRun(nullptr);
        GAP_Dbg("Services started, waiting for threads to join\n");
        // Wait for all services to exit
        for(ThreadPtr t : threadSet_) {
            if(t->joinable()) t->join();
        }
    }


    void IoServicePool::asyncRun(std::function<void()> cb) {
        for (WeightedService service: serviceSet_) {
            threadSet_.emplace_back(new boost::thread(boost::bind(&io_service::run, service.second)));
        }
        GAP_Dbg("Service pool started. %u io services running\n", poolSize_);
        if (cb != nullptr) {
            cb();
        }
    }

    void IoServicePool::stop() {
        for (WeightedService service: serviceSet_) {
            GAP_Dbg("Service[%p] scheduled=%u\n", service.second.get(), service.first);
            service.second->stop();
        }

        GAP_Dbg("Services stopped, waiting for threads to exit\n");
        // Wait for all services to exit
        for(ThreadPtr t : threadSet_) {
            if(t->joinable()) t->join();
        }
    }

    void IoServicePool::init(const uint poolSize = 0) {
        if (inst_ == nullptr) {
            uint concurrency  = poolSize;
            if (poolSize == 0) {
                concurrency = boost::thread::hardware_concurrency();
                // reserve one service to accepting connections
                if (concurrency > 1) {
                    concurrency--;
                }
            }

            inst_ = IoServicePool::Ptr(new IoServicePool(concurrency));
        }
    }

    IoServicePool::Ptr IoServicePool::instance() {
        if (inst_ == nullptr) {
            inst_ = IoServicePool::Ptr(new IoServicePool(boost::thread::hardware_concurrency()));
        }
        return inst_;
    }

    const uint IoServicePool::size() const {
        return poolSize_;
    }
}