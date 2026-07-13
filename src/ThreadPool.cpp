#include "ThreadPool.h"

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>


ThreadPool::ThreadPool(int size)
    :
      stop(false)
{

    for(int i = 0; i < size; i++)
    {
        workers.emplace_back(
            &ThreadPool::worker,
            this
        );
    }

}



void ThreadPool::worker()
{

    while(true)
    {

        int client_fd;


        {
            std::unique_lock<std::mutex> lock(mutex);


            condition.wait(
                lock,
                [this]()
                {
                    return stop || !tasks.empty();
                }
            );


            if(stop && tasks.empty())
            {
                return;
            }


            client_fd = tasks.front();

            tasks.pop();

        }


        handle_client(client_fd);
    }

}



void ThreadPool::submit(int client_fd)
{

    {
        std::lock_guard<std::mutex> lock(mutex);


        tasks.push(client_fd);
    }


    condition.notify_one();

}



void ThreadPool::handle_client(int client_fd)
{

    char buffer[1024];


    while(true)
    {

        int n = recv(
            client_fd,
            buffer,
            sizeof(buffer),
            0
        );


        if(n <= 0)
        {
            break;
        }


        send(
            client_fd,
            buffer,
            n,
            0
        );
    }


    close(client_fd);

}



ThreadPool::~ThreadPool()
{

    {
        std::lock_guard<std::mutex> lock(mutex);

        stop = true;
    }


    condition.notify_all();



    for(auto& worker : workers)
    {
        worker.join();
    }

}
