#include "Metrics.h"
#include "Logger.h"
#include "ThreadPool.h"

#include <iostream>

Metrics& Metrics::instance()
{
    static Metrics metrics;

    return metrics;
}

void Metrics::incrementConnections()
{
    connections_++;
}

void Metrics::decrementConnections()
{
    connections_--;
}

void Metrics::incrementRequests()
{
    requests_++;
}

void Metrics::incrementResponses()
{
    responses_++;
}

void Metrics::setThreadPool(ThreadPool* pool)
{
    threadPool_ = pool;
}

void Metrics::recordQueueWait(uint64_t us)
{
    totalQueueWait_ += us;

    completedTasks_++;
}

void Metrics::recordExecutionTime(uint64_t us)
{
    totalExecutionTime_ += us;
}

void Metrics::print()
{
    Logger::instance().log(
        "threadPool ptr = " +
        std::to_string(reinterpret_cast<uintptr_t>(threadPool_)));

    Logger::instance().log("========== Runtime Metrics =========="
                           "\nConnections: " +
                           std::to_string(connections_.load()) +
                           "\nRequests: " + std::to_string(requests_.load()) +
                           "\nResponses: " + std::to_string(responses_.load()));

    if (threadPool_)
    {
        Logger::instance().log("ThreadPool Queue Current: " +
                               std::to_string(threadPool_->queueSize()));

        Logger::instance().log("ThreadPool Queue Max: " +
                               std::to_string(threadPool_->maxQueueSize()));
    }

    if (completedTasks_ > 0)
    {
        Logger::instance().log(
            "Average Queue Wait(us): " +
            std::to_string(totalQueueWait_.load() / completedTasks_.load()));

        Logger::instance().log("Average Execution Time(us): " +
                               std::to_string(totalExecutionTime_.load() /
                                              completedTasks_.load()));
    }

    Logger::instance().log("Request Buffers: " +
                           std::to_string(requestBuffers_.load()));

    Logger::instance().log("Response Buffers: " +
                           std::to_string(responseBuffers_.load()));

    Logger::instance().log("Write Buffer Appends: " +
                           std::to_string(writeBufferAppends_.load()));

    Logger::instance().log("Rejected Requests: " +
                           std::to_string(rejectedRequests_.load()));

    Logger::instance().log("=====================================");
}

void Metrics::incrementAllocations()
{
    allocations_++;
}

void Metrics::incrementRequestBuffers()
{
    requestBuffers_++;
}

void Metrics::incrementResponseBuffers()
{
    responseBuffers_++;
}

void Metrics::incrementWriteBufferAppends()
{
    writeBufferAppends_++;
}

void Metrics::incrementRejectedRequests()
{
    rejectedRequests_++;
}

uint64_t Metrics::allocations() const
{
    return allocations_.load();
}
