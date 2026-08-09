#include "Metrics.h"
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
    std::cout << "\nthreadPool ptr = " << threadPool_ << std::endl;

    std::cout << "\n========== Runtime Metrics =========="
              << "\nConnections: " << connections_.load()
              << "\nRequests: " << requests_.load()
              << "\nResponses: " << responses_.load();

    if (threadPool_)
    {
        std::cout << "\nThreadPool Queue Current: " << threadPool_->queueSize();

        std::cout << "\nThreadPool Queue Max: " << threadPool_->maxQueueSize();
    }

    if (completedTasks_ > 0)
    {
        std::cout << "\nAverage Queue Wait(us): "
                  << totalQueueWait_.load() / completedTasks_.load();

        std::cout << "\nAverage Execution Time(us): "
                  << totalExecutionTime_.load() / completedTasks_.load();
    }

    std::cout << "\n=====================================\n";
}