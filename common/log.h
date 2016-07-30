//
// Created by dc on 6/2/16.
//

#ifndef GAR_LOG_H
#define GAR_LOG_H

#include<stdarg.h>
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

    enum ConsoleColor {
        BLACK = 0,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        RESET
    };

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

        template <typename... Args>
        static void console(FILE *fp, const ConsoleColor color, const char* tag, const char* fmt, Args... args) {
            static const char *colors[] = {
                    "\033[1m\x1B[30m",
                    "\033[1m\x1B[31m",
                    "\033[1m\x1B[32m",
                    "\033[1m\x1B[33m",
                    "\033[1m\x1B[34m",
                    "\033[1m\x1B[35m",
                    "\033[1m\x1B[36m",
                    "\033[1m\x1B[37m",
                    "\x1B[0m"
            };

#define COLOR_RESET "\x1B[0m"
            char   msg[GAP_LOGLINE_MAX_SIZE];
            int     size = 0;

            size = snprintf(msg, GAP_LOGLINE_MAX_SIZE, fmt, args...);
            msg[size] = '\0';

            fprintf(fp, "%s[%s] %s%s\n", colors[color], tag, msg, COLOR_RESET);
#undef COLOR_RESET

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

#ifdef SUIL_ENABLE_LOGSRC
#define GAR_LOGSRC_FMT  "%s<%s:%d>: "
#define GAR_LOGSRC_ARGS ,"" ,__FILENAME__, __LINE__
#else
#define GAR_LOGSRC_FMT  "%s"
#define GAR_LOGSRC_ARGS ,""
#endif


#define LOG(lvl, fmt, ...)  if (lvl >= gap::Logger::getLogLevel()) \
    gap::Logger::log(lvl, GAR_LOGSRC_FMT fmt GAR_LOGSRC_ARGS, ## __VA_ARGS__)

#ifdef SUIL_DEBUG
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

#define GAP_Err(fmt, ...)       LOG(gap::LogLevel::ERROR, fmt, ## __VA_ARGS__)
#define GAP_Crit(fmt, ...)      LOG(gap::LogLevel::CRITICAL, fmt, ## __VA_ARGS__)

#define CONSOLE(fp, tag, color, fmt, ...)   gap::Logger::console(fp, color, tag, "%s" fmt, "", ## __VA_ARGS__)
#define GAP_Put(tag, fmt, ...)  CONSOLE(stdout, tag, gap::ConsoleColor::WHITE, "%s" fmt, "", ## __VA_ARGS__)
#define GAP_Perr(tag, fmt, ...) CONSOLE(stderr, tag, gap::ConsoleColor::RED, "error: %s" fmt, "", ## __VA_ARGS__)
#define GAP_Pwrn(tag, fmt, ...) CONSOLE(stdout, tag, gap::ConsoleColor::YELLOW, "warning: %s" fmt, "", ## __VA_ARGS__)
#define GAP_Pinf(tag, fmt, ...) CONSOLE(stdout, tag, gap::ConsoleColor::CYAN, "%s" fmt, "", ## __VA_ARGS__)

}
#endif //GAR_LOG_H
