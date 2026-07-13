#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "ThreadPool.h"



int main()
{

    int server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    sockaddr_in server_addr{};


    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(8080);

    server_addr.sin_addr.s_addr = INADDR_ANY;



    bind(
        server_fd,
        (sockaddr*)&server_addr,
        sizeof(server_addr)
    );


    listen(server_fd,128);



    std::cout
        << "Server listening..."
        << std::endl;



    ThreadPool pool(4);



    while(true)
    {

        sockaddr_in client_addr{};

        socklen_t len =
            sizeof(client_addr);



        int client_fd = accept(
            server_fd,
            (sockaddr*)&client_addr,
            &len
        );


        std::cout
            << "New client\n";


        pool.submit(client_fd);

    }


    close(server_fd);

}