#include "utils/log.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

namespace
{
    struct LevelInfo
    {
        std::string_view name;
        std::string_view color;
    };

    LevelInfo getLevelInfo(LogLevel level)
    {
        switch (level)
        {
            using enum LogLevel;
        case TRACE:
            return {"TRACE", "\033[37m"};
        case DEBUG:
            return {"DEBUG", "\033[36m"};
        case INFO:
            return {"INFO", "\033[32m"};
        case WARNING:
            return {"WARNING", "\033[33m"};
        case ERROR:
            return {"ERROR", "\033[31m"};
        case CRITICAL:
            return {"CRITICAL", "\033[35m"};
        }
        throw std::invalid_argument("invalid log level");
    }

    bool supportsColor()
    {
        static bool result = isatty(fileno(stderr));
        return result;
    }
}

void ConsoleLogger::log(LogLevel level, std::string_view message)
{
    if (level < minLevel_)
        return;

    auto timestamp = std::format("{:%H:%M:%S}",
                                 std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()));

    auto info = getLevelInfo(level);

    std::lock_guard<std::mutex> lock(mutex_);

    if (supportsColor())
        std::cerr << info.color;

    std::cerr
        << "[" << timestamp << "]"
        << "[" << info.name << "]"
        << " - " << message;

    if (supportsColor())
        std::cerr << "\033[0m";

    std::cerr << "\n";
}

LogManager &LogManager::instance()
{
    static LogManager manager;
    return manager;
}

void LogManager::setLogger(std::unique_ptr<Logger> logger)
{
    logger_ = std::move(logger);
}

Logger &LogManager::logger()
{
    if (!logger_)
        logger_ = std::make_unique<ConsoleLogger>();
    return *logger_;
}
