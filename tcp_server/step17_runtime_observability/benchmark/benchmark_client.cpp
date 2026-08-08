#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <vector>

void runClient(int id);

int main()
{

    int clients = 10;

    std::vector<std::thread> threads;

    for (int i = 0; i < clients; i++)
    {
        threads.emplace_back(runClient, i);
    }

    for (auto& t : threads)
    {
        t.join();
    }
}

void runClient(int id)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        perror("socket");
        return;
    }

    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    char buffer[4096];

    if (connect(fd, (sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("connect");
        close(fd);
        return;
    }

    for (int i = 0; i < 100; i++)
    {
        std::string msg = "client-" + std::to_string(id);

        send(fd, msg.data(), msg.size(), 0);

        recv(fd, buffer, sizeof(buffer), 0);
    }

    close(fd);
}