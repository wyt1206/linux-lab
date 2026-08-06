#pragma once

#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
public:

    using Task =
        std::function<void()>;

    explicit ThreadPool(
        size_t threadNum
    );

    ~ThreadPool();

    void submit(
        Task task
    );

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
    std::mutex mutex_;

    std::condition_variable condition_;

    /*
        shutdown flag
    */
    bool stop_;
};