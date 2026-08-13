#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
  public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t threadNum);

    ~ThreadPool();

    bool submit(Task task);

    size_t queueSize() const;

    size_t maxQueueSize() const;

  private:
    void workerLoop();

  private:
    static constexpr size_t kMaxQueueSize = 100;
    /*
        worker threads
    */
    std::vector<std::thread> workers_;

    struct TaskItem
    {
        Task task;

        std::chrono::steady_clock::time_point enqueueTime;
    };

    /*
        task queue
    */
    std::queue<TaskItem> tasks_;

    /*
        synchronization
    */
    mutable std::mutex mutex_;

    std::condition_variable condition_;

    /*
        shutdown flag
    */
    bool stop_;

    std::atomic<size_t> maxQueueSize_{0};
};
