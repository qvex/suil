//
// Created by dc on 6/2/16.
//
#include "log.h"

namespace gap {

    void DefaultLogHandler::log(const LogLevel level, std::string msg) {
        std::cout << msg;
    }


    //-----------------------------------------
    // Log Handler
    //---------------------------------------------
    Logger::Ptr  Logger::inst_ = Logger::Ptr{nullptr};

    const ptime Logger::EPOCH = ptime(boost::gregorian::date(1970, 1, 1));

    Logger::Ptr Logger::instance() {
        if (Logger::inst_ == nullptr) {
            inst_ = Logger::Ptr(new Logger());
        }
        return inst_;
    }

    std::size_t Logger::prelog(const LogLevel level, std::string& buf) {
        auto now = boost::posix_time::microsec_clock::universal_time()-EPOCH;
        std::string levelStr;
        switch (level) {
            case LogLevel::TRACE:
                levelStr = "TRACE";
                break;
            case LogLevel::INFO:
                levelStr = "INFO";
                break;
            case LogLevel::DEBUG:
                levelStr = "DEBUG";
                break;
            case LogLevel::WARNING:
                levelStr = "WARNING";
                break;
            case LogLevel::ERROR:
                levelStr = "ERROR";
                break;
            case LogLevel::CRITICAL:
                levelStr = "CRITICAL";
                break;
        }
        std::string tid = boost::lexical_cast<std::string>(boost::this_thread::get_id());
        return sprintf(&buf[0], "[%10.10ld] [%10.10s] [%8.8s] ",
                       now.ticks(), tid.c_str(), levelStr.c_str());
    }
}
