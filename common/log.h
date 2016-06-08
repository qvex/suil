//
// Created by dc on 6/2/16.
//

#ifndef GAR_LOG_H
#define GAR_LOG_H

#include <memory>
#include <ctime>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/local_time/local_time.hpp>

namespace gap {

#ifndef GAP_LOGLINE_MAX_SIZE
#define GAP_LOGLINE_MAX_SIZE    255
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    using boost::posix_time::ptime;

    class LogHandler {
    public:
        typedef std::shared_ptr<LogHandler> Ptr;
        virtual void log(const LogLevel level, std::string msg) = 0;
    };

    class DefaultLogHandler : public LogHandler {
    public:
        virtual void log(const LogLevel level, std::string msg);
    };

    class Logger : public std::enable_shared_from_this<Logger>,
            public boost::noncopyable
    {
    public:
        typedef std::shared_ptr<Logger> Ptr;

        static void setLogLevel(const LogLevel level) {
            instance()->getLogLevelRef() = level;
        }


        static LogLevel getLogLevel() {
            return instance()->getLogLevelRef();
        }

        static void setHandler(LogHandler::Ptr handler) {
            if (handler != nullptr) {
                instance()->getLogHandlerRef() = handler;
            }
        }

        template <typename... Args>
        static void log(const LogLevel level, std::string&& msg, Args... args)
        {
            std::string logMsg(GAP_LOGLINE_MAX_SIZE, '\0');
            std::size_t size = prelog(level, logMsg);
            size += snprintf(&logMsg[size], GAP_LOGLINE_MAX_SIZE-size, msg.c_str(), args...);
            logMsg.resize(size);
            instance()->getLogHandlerRef()->log(level, std::move(logMsg));
        }

        static Logger::Ptr instance();

    private:
        LogLevel& getLogLevelRef() {
            return  level_;
        }

        LogHandler::Ptr& getLogHandlerRef() {
            if (handler_ == nullptr) {
                handler_ = LogHandler::Ptr((LogHandler *)new DefaultLogHandler());
            }
            return handler_;
        }

        static std::size_t prelog(const LogLevel level, std::string& log);

    private:
        LogHandler::Ptr     handler_{nullptr};
        LogLevel            level_{LogLevel::DEBUG};
        static const ptime  EPOCH;
        static Logger::Ptr  inst_;
    };

#ifdef GAR_ENABLE_LOGSRC
#define GAR_LOGSRC_FMT  "%s<%s:%d>: "
#define GAR_LOGSRC_ARGS ,"" ,__FILENAME__, __LINE__
#else
#define GAR_LOGSRC_FMT  "%s"
#define GAR_LOGSRC_ARGS ,""
#endif


#define LOG(lvl, fmt, ...)  if (lvl >= gap::Logger::getLogLevel()) \
    gap::Logger::log(lvl, GAR_LOGSRC_FMT fmt GAR_LOGSRC_ARGS, ## __VA_ARGS__)

#ifdef GAR_DEBUG
#define GAP_Trace(fmt, ...) LOG(gap::LogLevel::TRACE, fmt, ## __VA_ARGS__)
#define GAP_Dbg(fmt, ...)   LOG(gap::LogLevel::DEBUG, fmt, ## __VA_ARGS__)
#define GAP_Info(fmt, ...)  LOG(gap::LogLevel::INFO,  fmt, ## __VA_ARGS__)
#define GAP_Warn(fmt, ...)  LOG(gap::LogLevel::WARNING, fmt, ## __VA_ARGS__)
#else
#define GAP_Trace(fmt, ...)
#define GAP_Dbg(fmt, ...)
#define GAP_Info(fmt, ...)
#define GAP_Warn(fmt, ...)
#endif

#define GAP_Err(fmt, ...)   LOG(gap::LogLevel::ERROR, fmt, ## __VA_ARGS__)
#define GAP_Crit(fmt, ...)  LOG(gap::LogLevel::CRITICAL, fmt, ## __VA_ARGS__)

}
#endif //GAR_LOG_H
