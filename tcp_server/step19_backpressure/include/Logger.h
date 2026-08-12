#pragma once

#include <iostream>
#include <mutex>
#include <string>

class Logger
{

  public:
    static Logger& instance();

    void enable(bool enabled);

    void log(const std::string& message);

  private:
    Logger() = default;

  private:
    bool enabled_{true};

    std::mutex mutex_;
};
