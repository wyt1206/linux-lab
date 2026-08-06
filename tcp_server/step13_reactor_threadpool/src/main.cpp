#include "ThreadPool.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{

    ThreadPool pool(4);

    for (int i = 0; i < 10; i++)
    {

        pool.submit(
            [i]()
            {
                std::cout << "Task " << i << " running in thread "
                          << std::this_thread::get_id() << std::endl;

                std::this_thread::sleep_for(std::chrono::seconds(1));

                std::cout << "Task " << i << " finished" << std::endl;
            });
    }

    /*
        wait tasks finish

        temporary for test

    */

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}