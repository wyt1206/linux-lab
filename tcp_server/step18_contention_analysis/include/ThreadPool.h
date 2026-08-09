#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>

class ThreadPool
{
  public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t threadNum);

    ~ThreadPool();

    void submit(Task task);

    size_t queueSize() const;

    size_t maxQueueSize() const;

  private:
    void workerLoop();

  private:
    /*
        worker threads
    */
    std::vector<std::thread> workers_;

    /*
        task queue
    */
    std::queue<Task> tasks_;

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