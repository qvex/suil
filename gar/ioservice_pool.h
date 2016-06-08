//
// Created by dc on 6/2/16.
//

#ifndef GAR_IOSERVICE_POOL_H
#define GAR_IOSERVICE_POOL_H

#include <memory>
#include <vector>
#include <boost/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

namespace gar {

    using boost::asio::io_service;
    typedef std::shared_ptr<boost::thread>          ThreadPtr;
    typedef std::shared_ptr<io_service::work>       WorkPtr;
    typedef std::shared_ptr<io_service>   ServicePtr;

    class IoServicePool : public std::enable_shared_from_this<IoServicePool>,
            public boost::noncopyable
    {
    public:
        typedef std::shared_ptr<IoServicePool>  Ptr;
        static void init(const uint poolSize);
        static IoServicePool::Ptr instance();
        ServicePtr select(const uint index);
        ServicePtr schedule();
        void run();
        void asyncRun(std::function<void()> cb);
        void stop();
        template <typename Work>
        void schedule(Work handler);
        ServicePtr  reserved();
        const uint size() const;
    private:
        IoServicePool(const uint poolSize = 0);
    private:
        typedef std::pair<uint, ServicePtr> WeightedService;
        const uint                      poolSize_;
        uint                            nextService_;
        std::vector<WeightedService>    serviceSet_;
        std::vector<WorkPtr>            workSet_;
        std::vector<ThreadPtr>          threadSet_;
        ServicePtr                      server_;
        static IoServicePool::Ptr       inst_;
    };

    template <typename Work>
    void IoServicePool::schedule(Work work){
        this->schedule()->post(work);
    }
}
#endif //GAR_IOSERVICE_POOL_H
