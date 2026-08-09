#include "Logger.h"

Logger& Logger::instance()
{
    static Logger logger;

    return logger;
}

void Logger::log(const std::string& message)
{

    std::lock_guard<std::mutex> lock(mutex_);

    std::cout << message << std::endl;
}