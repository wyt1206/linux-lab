#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


class ThreadPool
{

public:

    ThreadPool(int size);


    ~ThreadPool();


    void submit(int client_fd);



private:

    void worker();


    void handle_client(int client_fd);



private:

    std::vector<std::thread> workers;


    std::queue<int> tasks;


    std::mutex mutex;


    std::condition_variable condition;


    bool stop;

};


#endif
