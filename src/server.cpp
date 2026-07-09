#include <iostream>

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


    if (server_fd == -1)
    {
        std::cerr << "socket failed\n";
        return 1;
    }


    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);



    int result = bind(
        server_fd,
        (struct sockaddr*)&address,
        sizeof(address)
    );


    if (result == -1)
    {
        std::cerr << "bind failed\n";
        close(server_fd);
        return 1;
    }


    std::cout << "Bind success\n";


    close(server_fd);

    return 0;
}