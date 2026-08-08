#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>

int main()
{

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(fd, (sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("connect");
        return -1;
    }

    std::string message = "hello";

    send(fd, message.data(), message.size(), 0);

    char buffer[4096];

    int n = recv(fd, buffer, sizeof(buffer), 0);

    std::cout << "response: " << std::string(buffer, n) << std::endl;

    close(fd);

    return 0;
}