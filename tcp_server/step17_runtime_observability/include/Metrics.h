#pragma once

#include <atomic>
#include <cstdint>

class Metrics
{

  public:
    static Metrics& instance();

    void incrementConnections();

    void decrementConnections();

    void incrementRequests();

    void incrementResponses();

    void print();

  private:
    Metrics() = default;

  private:
    std::atomic<uint64_t> connections_{0};

    std::atomic<uint64_t> requests_{0};

    std::atomic<uint64_t> responses_{0};
};