#include "ThreadPool.h"



ThreadPool::ThreadPool(size_t numThreads)
    : stop(false)
{

    for(size_t i = 0; i < numThreads; i++)
    {

        workers.emplace_back(

            [this]()
            {

                while(true)
                {

                    std::function<void()> task;


                    {
                        std::unique_lock<std::mutex> lock(queueMutex);


                        /*
                         * 如果：
                         * 1. stop == true
                         * 或者
                         * 2. queue里面有任务
                         *
                         * worker继续执行
                         *
                         * 否则睡眠
                         */
                        condition.wait(
                            lock,
                            [this]()
                            {
                                return stop || !tasks.empty();
                            }
                        );


                        // thread pool shutdown
                        if(stop && tasks.empty())
                        {
                            return;
                        }


                        // get task
                        task = std::move(tasks.front());

                        tasks.pop();

                    }


                    // execute task
                    task();

                }

            }

        );

    }

}



void ThreadPool::enqueue(
    std::function<void()> task
)
{

    {

        std::lock_guard<std::mutex> lock(queueMutex);


        tasks.push(
            std::move(task)
        );

    }


    // wake one sleeping worker
    condition.notify_one();

}



ThreadPool::~ThreadPool()
{

    {

        std::lock_guard<std::mutex> lock(queueMutex);

        stop = true;

    }


    // wake all workers
    condition.notify_all();



    // wait workers exit
    for(auto& worker : workers)
    {
        worker.join();
    }

}