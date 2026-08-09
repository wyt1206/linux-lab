#include "ThreadPool.h"

#include <utility>

ThreadPool::ThreadPool(size_t threadNum) : stop_(false)
{

    for (size_t i = 0; i < threadNum; i++)
    {

        workers_.emplace_back([this]() { workerLoop(); });
    }
}

ThreadPool::~ThreadPool()
{
    {

        std::lock_guard<std::mutex> lock(mutex_);

        stop_ = true;
    }

    /*
        wake up workers
    */
    condition_.notify_all();

    /*
        wait all threads
    */
    for (auto& worker : workers_)
    {

        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::submit(Task task)
{
    size_t currentSize;

    TaskItem item;

    item.task = std::move(task);

    item.enqueueTime = std::chrono::steady_clock::now();

    {

        std::lock_guard<std::mutex> lock(mutex_);

        tasks_.push(std::move(item));

        currentSize = tasks_.size();
    }

    size_t oldMax = maxQueueSize_.load();

    while (currentSize > oldMax &&
           !maxQueueSize_.compare_exchange_weak(oldMax, currentSize))
    {
    }

    /*
        notify one worker
    */
    condition_.notify_one();
}

void ThreadPool::workerLoop()
{

    while (true)
    {
        TaskItem item;

        {
            std::unique_lock<std::mutex> lock(mutex_);

            condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty())
            {
                return;
            }

            item = std::move(tasks_.front());

            tasks_.pop();
        }

        auto start = std::chrono::steady_clock::now();

        auto queueWait = std::chrono::duration_cast<std::chrono::microseconds>(
                             start - item.enqueueTime)
                             .count();

        if (item.task)
        {
            item.task();
        }
    }
}

size_t ThreadPool::queueSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return tasks_.size();
}

size_t ThreadPool::maxQueueSize() const
{
    return maxQueueSize_.load();
}