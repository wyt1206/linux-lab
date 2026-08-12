#pragma once

#include <atomic>
#include <cstdint>

class ThreadPool;

class Metrics
{

  public:
    static Metrics& instance();

    void incrementConnections();

    void decrementConnections();

    void incrementRequests();

    void incrementResponses();

    void recordQueueWait(uint64_t us);

    void recordExecutionTime(uint64_t us);

    void setThreadPool(ThreadPool* pool);

    void print();

    void incrementAllocations();

    void incrementRequestBuffers();

    void incrementResponseBuffers();

    void incrementWriteBufferAppends();

    uint64_t allocations() const;

  private:
    Metrics() = default;

  private:
    std::atomic<uint64_t> connections_{0};

    std::atomic<uint64_t> requests_{0};

    std::atomic<uint64_t> responses_{0};

    std::atomic<uint64_t> totalQueueWait_{0};

    std::atomic<uint64_t> completedTasks_{0};

    std::atomic<uint64_t> totalExecutionTime_{0};

    std::atomic<uint64_t> allocations_{0};

    std::atomic<uint64_t> requestBuffers_{0};

    std::atomic<uint64_t> responseBuffers_{0};

    std::atomic<uint64_t> writeBufferAppends_{0};

    ThreadPool* threadPool_{nullptr};
};
