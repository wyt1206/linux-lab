#include "Logger.h"

Logger& Logger::instance()
{
    static Logger logger;

    return logger;
}

void Logger::log(const std::string& message)
{
    if (!enabled_)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    std::cout << message << std::endl;
}

void Logger::enable(bool enabled)
{
    enabled_ = enabled;
}
