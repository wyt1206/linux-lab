#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t thread_num) : stop(false)
{

    for (size_t i = 0; i < thread_num; i++)
    {

        workers.emplace_back(
            [this]()
            {
                while (true)
                {

                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);

                        condition.wait(lock, [this]()
                                       { return stop || !tasks.empty(); });

                        if (stop && tasks.empty())
                        {
                            return;
                        }

                        task = std::move(tasks.front());

                        tasks.pop();
                    }

                    // 执行任务
                    task();
                }
            });
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{

    {
        std::lock_guard<std::mutex> lock(queue_mutex);

        tasks.push(std::move(task));
    }

    condition.notify_one();
}

ThreadPool::~ThreadPool()
{

    {
        std::lock_guard<std::mutex> lock(queue_mutex);

        stop = true;
    }

    condition.notify_all();

    for (auto& worker : workers)
    {
        worker.join();
    }
}