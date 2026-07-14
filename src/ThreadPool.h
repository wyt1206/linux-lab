#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>


class ThreadPool
{

public:

    ThreadPool(size_t numThreads);

    ~ThreadPool();


    void enqueue(std::function<void()> task);


private:

    // worker threads
    std::vector<std::thread> workers;


    // task queue
    std::queue<std::function<void()>> tasks;


    // protect task queue
    std::mutex queueMutex;


    // notify workers
    std::condition_variable condition;


    // stop flag
    bool stop;


};


#endif