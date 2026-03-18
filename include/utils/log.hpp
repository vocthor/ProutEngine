#pragma once

#include <format>
#include <memory>
#include <mutex>
#include <string_view>

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
};

// TODO: add file logger
// ! TODO: get caller function name and line number

class Logger
{
public:
    virtual ~Logger() = default;
    virtual void log(LogLevel level, std::string_view message) = 0;
};

class ConsoleLogger : public Logger
{
public:
    explicit ConsoleLogger(LogLevel minLevel = LogLevel::TRACE)
        : minLevel_{minLevel} {}

    void log(LogLevel level, std::string_view message) override;

private:
    LogLevel minLevel_;
    std::mutex mutex_;
};

class LogManager
{
public:
    static LogManager &instance();

    void setLogger(std::unique_ptr<Logger> logger);
    Logger &logger();

private:
    LogManager() = default;
    std::unique_ptr<Logger> logger_;
};

namespace Log
{
    template <typename... Args>
    void trace(std::format_string<Args...> fmt, Args &&...args)
    {
        LogManager::instance().logger().log(LogLevel::TRACE, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void debug(std::format_string<Args...> fmt, Args &&...args)
    {
        LogManager::instance().logger().log(LogLevel::DEBUG, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args &&...args)
    {
        LogManager::instance().logger().log(LogLevel::INFO, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warning(std::format_string<Args...> fmt, Args &&...args)
    {
        LogManager::instance().logger().log(LogLevel::WARNING, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args &&...args)
    {
        LogManager::instance().logger().log(LogLevel::ERROR, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void critical(std::format_string<Args...> fmt, Args &&...args)
    {
        LogManager::instance().logger().log(LogLevel::CRITICAL, std::format(fmt, std::forward<Args>(args)...));
    }
} // namespace Log
