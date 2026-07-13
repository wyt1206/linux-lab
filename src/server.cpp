#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


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
        << "Listening...\n";


    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);


    int client_fd = accept(
        server_fd,
        (sockaddr*)&client_addr,
        &client_len
    );


    std::cout 
        << "Client connected\n";


    char buffer[1024];


    int n = recv(
        client_fd,
        buffer,
        sizeof(buffer),
        0
    );


    if (n > 0)
    {
        std::cout 
            << "Received: ";

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

    close(server_fd);


    return 0;
}