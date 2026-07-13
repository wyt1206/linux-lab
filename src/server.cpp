#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


void handle_client(int client_fd)
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
            std::cout 
                << "Client disconnected\n";

            break;
        }


        std::cout << "Received: ";

        std::cout.write(
            buffer,
            n
        );

        std::cout << std::endl;


        send(
            client_fd,
            buffer,
            n,
            0
        );
    }


    close(client_fd);
}



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


    listen(server_fd, 128);


    std::cout 
        << "Server listening...\n";


    while(true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);


        int client_fd = accept(
            server_fd,
            (sockaddr*)&client_addr,
            &client_len
        );


        if(client_fd < 0)
        {
            perror("accept");
            continue;
        }


        std::cout 
            << "New client connected\n";


        handle_client(client_fd);
    }


    close(server_fd);

    return 0;
}