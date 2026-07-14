#include "ThreadPool.h"

#include <chrono>


int main()
{


    ThreadPool pool(3);



    for(int i = 0; i < 10; i++)
    {

        pool.enqueue(

            [i]()
            {

                std::cout
                    << "Task "
                    << i
                    << " running in thread "
                    << std::this_thread::get_id()
                    << std::endl;


                std::this_thread::sleep_for(
                    std::chrono::seconds(1)
                );


                std::cout
                    << "Task "
                    << i
                    << " finished"
                    << std::endl;

            }

        );

    }



    std::this_thread::sleep_for(
        std::chrono::seconds(5)
    );


    return 0;
}