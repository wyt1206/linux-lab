#pragma once

#include <iostream>
#include <mutex>
#include <string>

class Logger
{

  public:
    static Logger& instance();

    void log(const std::string& message);

  private:
    Logger() = default;

  private:
    std::mutex mutex_;
};