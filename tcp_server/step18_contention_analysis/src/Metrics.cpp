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

void Metrics::print()
{
    std::cout << "\nthreadPool ptr = " << threadPool_ << std::endl;

    std::cout << "\n========== Runtime Metrics =========="
              << "\nConnections: " << connections_.load()
              << "\nRequests: " << requests_.load()
              << "\nResponses: " << responses_.load();

    if (threadPool_)
    {
        std::cout << "\nThreadPool Queue: " << threadPool_->queueSize();
    }

    std::cout << "\n=====================================\n";
}